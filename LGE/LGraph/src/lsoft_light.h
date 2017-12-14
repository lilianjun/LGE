#pragma once

#include "lmath.hpp"

//////////////////////////////////////////////////////////////////////////
// 光照 提供了一些光照算法


// 平行光
struct ldir_light
{
	// 光线方向
	float3 dirction;
	// 光的颜色
	lcolor clr;
	// 光的强度
	float power;
};

// 线性点光源
struct lpoint_light
{
	// 位置
	float3 pos;
	// 光的颜色
	lcolor clr;
	// 光照强度
	float power;
	// 范围 距离超出此范围，就不接受光照
	float range;
	// 常量、线性、平方系数
	float atten1, atten2, atten3;
};



// TODO: Atten = 1/( att0i + att1i * d + att2i * d2)



// 求被点光源照射的强度 ltpos:光源位置 v:顶点位置
float attenuation(const float3& ltpos, const float3& v)
{

	return  1.f / lengthsqr(ltpos - v);
}


// Lambert 求diffuse光照
// 输入：光的方向 顶点法线
// L：顶点到光源向量
inline float ldiffuse_lambert(const float3& N, const float3& L)
{
	return  saturate(dot(N, L));
}


// 求高光 phong 模式
// N：顶点法线 V：顶点到视点向量 L：顶点到光源向量
inline float lspecular_phong(const float3& N, const float3& L, const float3& V,  float _pow)
{
	float3 R = reflect(-L, N);
	float RVdot = dot(R, V);
	return pow(saturate(RVdot), _pow);
}

// blinn_phong求高光
// dot(N, H) ^ _pow
// vH = nor(vL + vV)
inline float lspecular_blinn(const float3& N, const float3& L, const float3& V,  float _pow)
{
	float3 H = normalize(V + L);
	float NHdot = dot(N, H);
	return pow(saturate(NHdot), _pow);
}

// 场景光 模板参数npl：点光源数量
// 一个场景的光照，有一个环境光 一个方向光 若干个点光源
struct lscene_light
{
	// 环境光
	lcolor ambient;
	// 方向光
	std::vector<ldir_light> dir_lts;
	// 点光源数组
	std::vector<lpoint_light> point_lts;
	// 环境光强度
	float ambient_power;
	// 漫反射光强度
	float diffuse_power;
	// 高光强度
	float specular_power;
};

// 光照器
// 构造：场景光
struct ltlighter
{
	lscene_light* sclt;

	// 只进行环境光照
	lcolor amb()
	{
		return saturate(sclt->ambient * sclt->ambient_power);
	}

	// 带漫反射光
	// 输入：顶点位置 顶点法线 
	// 输出：只有环境光和漫反射光的光照结果
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
			// 求衰减
			float3 L = pl.pos - pos;
			float dis = L.len();
			if (dis > pl.range)
			{
				continue;
			}
			// 衰减系数
			float atten = pl.atten1 + pl.atten2 / dis +  pl.atten3 / (dis * dis);
			// 光照结果
			L.setnor();
			float inten = ldiffuse_lambert(pos, L);
			// 决定最终光照结果的系数：距离影响的衰减 方向影响的强度 光源的强度
			dif_pt += saturate( atten * inten * pl.power * pl.clr);
		}
		// 环境光 漫反射光  相加得到最终光照结果
		return saturate(sclt->ambient * sclt->ambient_power + (dif_dir + dif_pt) * sclt->diffuse_power);
	}

	// // 输入：顶点位置 顶点法线 视点位置
	// 输出：环境光、漫反射光、高光相加的结果。
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
			// 求衰减
			float3 L = pl.pos - pos;
			float dis = L.len();
			if (dis > pl.range)
			{
				continue;
			}
			// 衰减系数
			// 衰减系数
			float atten = pl.atten1 + pl.atten2 / dis + pl.atten3 / (dis * dis);
			// 光照结果
			L.setnor();
			float inten = ldiffuse_lambert(nor, L);
			// 决定最终光照结果的系数：距离影响的衰减 方向影响的强度 光源的强度
			dif_pt += saturate(atten * inten * pl.power * pl.clr);
			// 求高光
			float inten_spe = lspecular_phong(nor, L, postoview,  spe_power);
			spe_pt += saturate( atten * inten_spe * pl.power * pl.clr );
		}
		// 环境光 漫反射光 高光  相加得到最终光照结果
		return saturate(sclt->ambient * sclt->ambient_power + (dif_dir + dif_pt) * sclt->diffuse_power
		+ (spe_dir + spe_pt) * sclt->specular_power );
	}
};
