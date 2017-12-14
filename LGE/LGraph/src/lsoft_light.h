#pragma once

#include "lmath.hpp"

//////////////////////////////////////////////////////////////////////////
// ���� �ṩ��һЩ�����㷨


// ƽ�й�
struct ldir_light
{
	// ���߷���
	float3 dirction;
	// �����ɫ
	lcolor clr;
	// ���ǿ��
	float power;
};

// ���Ե��Դ
struct lpoint_light
{
	// λ��
	float3 pos;
	// �����ɫ
	lcolor clr;
	// ����ǿ��
	float power;
	// ��Χ ���볬���˷�Χ���Ͳ����ܹ���
	float range;
	// ���������ԡ�ƽ��ϵ��
	float atten1, atten2, atten3;
};



// TODO: Atten = 1/( att0i + att1i * d + att2i * d2)



// �󱻵��Դ�����ǿ�� ltpos:��Դλ�� v:����λ��
float attenuation(const float3& ltpos, const float3& v)
{

	return  1.f / lengthsqr(ltpos - v);
}


// Lambert ��diffuse����
// ���룺��ķ��� ���㷨��
// L�����㵽��Դ����
inline float ldiffuse_lambert(const float3& N, const float3& L)
{
	return  saturate(dot(N, L));
}


// ��߹� phong ģʽ
// N�����㷨�� V�����㵽�ӵ����� L�����㵽��Դ����
inline float lspecular_phong(const float3& N, const float3& L, const float3& V,  float _pow)
{
	float3 R = reflect(-L, N);
	float RVdot = dot(R, V);
	return pow(saturate(RVdot), _pow);
}

// blinn_phong��߹�
// dot(N, H) ^ _pow
// vH = nor(vL + vV)
inline float lspecular_blinn(const float3& N, const float3& L, const float3& V,  float _pow)
{
	float3 H = normalize(V + L);
	float NHdot = dot(N, H);
	return pow(saturate(NHdot), _pow);
}

// ������ ģ�����npl�����Դ����
// һ�������Ĺ��գ���һ�������� һ������� ���ɸ����Դ
struct lscene_light
{
	// ������
	lcolor ambient;
	// �����
	std::vector<ldir_light> dir_lts;
	// ���Դ����
	std::vector<lpoint_light> point_lts;
	// ������ǿ��
	float ambient_power;
	// �������ǿ��
	float diffuse_power;
	// �߹�ǿ��
	float specular_power;
};

// ������
// ���죺������
struct ltlighter
{
	lscene_light* sclt;

	// ֻ���л�������
	lcolor amb()
	{
		return saturate(sclt->ambient * sclt->ambient_power);
	}

	// ���������
	// ���룺����λ�� ���㷨�� 
	// �����ֻ�л�������������Ĺ��ս��
	inline lcolor amb_dif(const float3& pos, const float3& nor)
	{
		lcolor dif_dir = lcolor::black();
		for (auto & dl : sclt->dir_lts)
		{
			dif_dir += saturate( dl.power * ldiffuse_lambert(nor, -dl.dirction) * dl.clr );
		}
		
		lcolor dif_pt = lcolor::black();
		for (const auto& pl : sclt->point_lts)
		{
			// ��˥��
			float3 L = pl.pos - pos;
			float dis = L.len();
			if (dis > pl.range)
			{
				continue;
			}
			// ˥��ϵ��
			float atten = pl.atten1 + pl.atten2 / dis +  pl.atten3 / (dis * dis);
			// ���ս��
			L.setnor();
			float inten = ldiffuse_lambert(pos, L);
			// �������չ��ս����ϵ��������Ӱ���˥�� ����Ӱ���ǿ�� ��Դ��ǿ��
			dif_pt += saturate( atten * inten * pl.power * pl.clr);
		}
		// ������ �������  ��ӵõ����չ��ս��
		return saturate(sclt->ambient * sclt->ambient_power + (dif_dir + dif_pt) * sclt->diffuse_power);
	}

	// // ���룺����λ�� ���㷨�� �ӵ�λ��
	// ����������⡢������⡢�߹���ӵĽ����
	inline lcolor amb_dif_spe(const float3& pos, const float3& nor, const float3& view_pos, float spe_power = 2.f)
	{
		// 
		lcolor dif_dir = lcolor::black();
		lcolor spe_dir = lcolor::black();
		float3 postoview = (view_pos - pos).setnor();
		for (auto & dl : sclt->dir_lts)
		{
			dif_dir += saturate(dl.power * ldiffuse_lambert(nor, -dl.dirction) * dl.clr);
			spe_dir += saturate( dl.power * lspecular_phong(nor, -dl.dirction, postoview, spe_power) * dl.clr);
		}

		lcolor dif_pt = lcolor::black();
		lcolor spe_pt = lcolor::black();
		for (const auto& pl : sclt->point_lts)
		{
			// ��˥��
			float3 L = pl.pos - pos;
			float dis = L.len();
			if (dis > pl.range)
			{
				continue;
			}
			// ˥��ϵ��
			// ˥��ϵ��
			float atten = pl.atten1 + pl.atten2 / dis + pl.atten3 / (dis * dis);
			// ���ս��
			L.setnor();
			float inten = ldiffuse_lambert(nor, L);
			// �������չ��ս����ϵ��������Ӱ���˥�� ����Ӱ���ǿ�� ��Դ��ǿ��
			dif_pt += saturate(atten * inten * pl.power * pl.clr);
			// ��߹�
			float inten_spe = lspecular_phong(nor, L, postoview,  spe_power);
			spe_pt += saturate( atten * inten_spe * pl.power * pl.clr );
		}
		// ������ ������� �߹�  ��ӵõ����չ��ս��
		return saturate(sclt->ambient * sclt->ambient_power + (dif_dir + dif_pt) * sclt->diffuse_power
		+ (spe_dir + spe_pt) * sclt->specular_power );
	}
};
