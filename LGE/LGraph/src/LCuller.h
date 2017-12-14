#pragma once

#include "lmath.hpp"

// �ü��� ����������� �ж϶����Ƿ������߻���
// �ж����̣���һ�� ����BOX�жϣ�����LOD�� �ڶ��� ���������ж�
// ��ȫ����׶����ģ��ͱ�CULL
// һ�����⣬һ�����ڵģ��Ƿ���вü�������
// ��ȫ���ڵģ�ͨ������
// �Լ�����ü��� �жϷ�ʽ�����������ڵ���ķ��ߣ����ֶ��������ߵķ��ߵļн�
// ��ִ����һ����
class LCuller
{
public:
	// �������������
	void SetCamera();

	int CullBox(const lboxf& box);

	// �����βü� 
	int CullTri(const float3& p1, const float3& p2, const float3& p3)
	{
		// TODO 
		return true;
	}

	// �ü�ͶӰ��������� ����������������
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