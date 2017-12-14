#pragma once

#include "lmath.hpp"

struct LDirLit 
{
};

// 顶点光照器
class LLighter
{
public:
	// 设置方向光
	void SetDirLight();

	// 设置环境光
	void SetAmbient();

	// 进行顶点光照
	// 输入顶点和法线，输出光照后的颜色
	void LightVertex(const float3* pver_int, const float3* pnor, int nver, largb* pcl_out);

	largb LightVertex(const float3& ver, const float3& nor, const largb& cl)
	{
		assert(0);
		return 0;
	}
};

// 像素级的光照器
class LLighterPixel
{

};