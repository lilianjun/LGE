#pragma once

#include "lmath.hpp"

struct LDirLit 
{
};

// ���������
class LLighter
{
public:
	// ���÷����
	void SetDirLight();

	// ���û�����
	void SetAmbient();

	// ���ж������
	// ���붥��ͷ��ߣ�������պ����ɫ
	void LightVertex(const float3* pver_int, const float3* pnor, int nver, largb* pcl_out);

	largb LightVertex(const float3& ver, const float3& nor, const largb& cl)
	{
		assert(0);
		return 0;
	}
};

// ���ؼ��Ĺ�����
class LLighterPixel
{

};