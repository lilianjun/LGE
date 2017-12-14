#pragma once

#include "lmath.hpp"

// 3D点的顶点变换器 可执行世界、视图、投影、视口等转换

// 模型变换 将物体的顶点从局部坐标变换到世界坐标或者摄像机坐标
// 执行此变换后，才能进行光照、碰撞检测、遮挡剔除、排序等
class LVertexTransformer
{
public:
	LVertexTransformer()
	{ }

	// 矩阵
	lmatrix4f m_matWorld;
	lmatrix4f m_matView;
	lmatrix4f m_matProj;
	lmatrix4f m_matWorldView;

	//
	float znear, zfar;
	int w, h;

	// 顶点变换到view空间 可用此变换网格顶点、包围盒等
	void TransVerModel(const float3* pver_in, int nver, float3* pver_out)
	{
		// todo
		for (int i = 0; i < nver; i++ )
		{
			pver_out[i] = m_matWorldView.trans_point( pver_in[i]);
			//pver_out[i] = m_matWorld.trans_point( pver_in[i]);
		}
	}

	// 投影变换进行投影并变换到BUFFER空间
	// 这里变换后输入的顶点，为TransVerModel之后的，即在摄像机空间的
	// 输出的x、y为整数，对应BUFFER上的坐标
	void TransVerProj(const float3* pver_in, int nver, float3* pver_out);

	// 一次性执行TransVerModel 和 TransVerProj
	// 在不用光照等的情况下，用这个接口
	void TransVerModelProj(const float3* pver_in, int nver, float3* pver_out);
	

	// 法线变换 变换到view坐标
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
