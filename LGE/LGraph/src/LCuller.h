#pragma once

#include "lmath.hpp"

// 裁剪器 包括摄像机： 判断对象是否在射线机内
// 判断流程：第一级 物体BOX判断（包括LOD） 第二级 逐三角形判断
// 完全在视锥体外的，就被CULL
// 一半在外，一半在内的，是否进行裁剪操作？
// 完全在内的，通过测试
// 以及背面裁剪器 判断方式：三角形所在的面的法线（左手定则）与视线的法线的夹角
// 先执行哪一个？
class LCuller
{
public:
	// 设置摄像机参数
	void SetCamera();

	int CullBox(const lboxf& box);

	// 三角形裁剪 
	int CullTri(const float3& p1, const float3& p2, const float3& p3)
	{
		// TODO 
		return true;
	}

	// 裁剪投影后的三角形 会生成若干三角形
	// x:[-1 1] y[-1 1] z[1, 0]
	auto CullProjTri(const ltri3df& tri)
	{

		static lboxf box(float3(-1.f, -1.f, 0), float3(1.f, 1.f, 1.f));
		// 
		return box.cull(tri);
	}

	// 
	std::vector<stVPosCl> CullProjTri(std::vector<stVPosCl>& verts)
	{
		return verts;
	}

	int CullBackTri(const ltri3df& tri);
};