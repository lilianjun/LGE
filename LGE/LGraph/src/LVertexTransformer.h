#pragma once

#include "lmath.hpp"

// 3D��Ķ���任�� ��ִ�����硢��ͼ��ͶӰ���ӿڵ�ת��

// ģ�ͱ任 ������Ķ���Ӿֲ�����任����������������������
// ִ�д˱任�󣬲��ܽ��й��ա���ײ��⡢�ڵ��޳��������
class LVertexTransformer
{
public:
	LVertexTransformer()
	{ }

	// ����
	lmatrix4f m_matWorld;
	lmatrix4f m_matView;
	lmatrix4f m_matProj;
	lmatrix4f m_matWorldView;

	//
	float znear, zfar;
	int w, h;

	// ����任��view�ռ� ���ô˱任���񶥵㡢��Χ�е�
	void TransVerModel(const float3* pver_in, int nver, float3* pver_out)
	{
		// todo
		for (int i = 0; i < nver; i++ )
		{
			pver_out[i] = m_matWorldView.trans_point( pver_in[i]);
			//pver_out[i] = m_matWorld.trans_point( pver_in[i]);
		}
	}

	// ͶӰ�任����ͶӰ���任��BUFFER�ռ�
	// ����任������Ķ��㣬ΪTransVerModel֮��ģ�����������ռ��
	// �����x��yΪ��������ӦBUFFER�ϵ�����
	void TransVerProj(const float3* pver_in, int nver, float3* pver_out);

	// һ����ִ��TransVerModel �� TransVerProj
	// �ڲ��ù��յȵ�����£�������ӿ�
	void TransVerModelProj(const float3* pver_in, int nver, float3* pver_out);
	

	// ���߱任 �任��view����
	void TransNorModel(const float3* pver_in, int nver, float3* pver_out);
	
	inline float3 TransNorModel(const float3& nor)
	{
		return m_matWorldView.trans_vector(nor);
	}

	inline float3 TransPointProj(const float3& ver)
	{
		float4 f4 = m_matProj.trans_point(float4(ver, 1.f));
		return float3(f4.x / f4.w * w + w/2, f4.y / f4.w * h + h/2 , f4.z / f4.w);
		//return float3(ver.x * 2 * znear / ver.z + w / 2, ver.y * 2 * znear / ver.z + h / 2,
		//	(ver.z - znear) / (zfar - znear));
		//return float3(ver.x / ver.z + w / 2, ver.y / ver.z + h / 2, ver.z);
	}

	void SetWorld(const float3& pos = float3(0.f, 0.f, 0.f),
		const float3& rot = float3(0.f, 0.f, 0.f),
		const float3& scl = float3(1.f, 1.f, 1.f))
	{
		m_matWorld.settrans(pos, rot, scl);
		m_matWorldView = m_matWorld * m_matView;
	}

	void SetView(const float3& pos, const float3& up, const float3& dir)
	{
		m_matView.setview(pos, up, dir);
		m_matWorldView = m_matWorld * m_matView;
	}

	void SetProj(float fov, float _aspect, float Znear, float Zfar)
	{
		m_matProj.setproj(fov, _aspect, Znear, Zfar);
		znear = Zfar;
		zfar = Zfar;
	}


};
