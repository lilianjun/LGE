#pragma once


#include "lmath.hpp"
#include "lstream_processor.hpp"

//////////////////////////////////////////////////////////////////////////
// �����λ�����



// ���������λ�ȡ������ 
// ���죺�洢�����ĳ�
// ���룺����ֵ
// �����������
template <typename t_pool>
struct ltri_getter
{
	lindex_getter<t_pool> index_getter;;

	// 
	ltri3df operator()(const lint3& i3)
	{
		return ltri3df(index_getter(i3.x), index_getter(i3.y), index_getter(i3.z));
	}
};

// ������任���� 
// ���룺���� �任����
// ������任��Ķ���
// ���������ĳ˷� �����ڶ��������任���Ӿ��任��ͶӰ�任��
struct lvertex_transformer
{
	lmatrix4f mat;

	float3 operator()(const float3& vertex)
	{
		return mat.trans_point(vertex);
	}

	float4 operator()(const float4& vertex)
	{
		return mat.trans_point(vertex);
	}
};

// �������б������
// ���룺������ ���������
template <typename t_buffer>
struct ltrilist_drawer 
{
	t_buffer buffer;


	template <typename t_pool>
	bool operator()(t_pool vertexdata)
	{
		// �����Ͷ�����ʹ��

		// ���������⣺ÿ������ṹҪ�ṩ��Ӧ�Ĵ����� ������ɫ�� ��������ɫ��
		// ���ַ������ ����Ⱦ���̿ɿ��Խ���

		// ��һ�׶� ��������任

		// �ڶ��׶� 

		// �ڶ��׶� ����������
	}
};


// �����任��
struct lvector_transformer
{
	lmatrix4f mat;

	float3 operator()(const float3& _vector)
	{
		return mat.trans_vector(_vector);
	}
};

// world_view��ת����
auto ltrans_world_view(const lmatrix4f& matworld, const lmatrix4f& matview)
{
	return lconnect(lvertex_transformer{ matworld }, lvertex_transformer{ matview });
}


// �������βü�����
// ���룺���ü������� box�ü���
// ������ü���������� ��0��������
struct ltri_culler
{
	lboxf box;
	std::vector<ltri3df> operator()(const ltri3df& tir_in);
};

// ���������δ�����

