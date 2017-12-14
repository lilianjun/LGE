#pragma once


#include "lmath.hpp"
#include "lstream_processor.hpp"

//////////////////////////////////////////////////////////////////////////
// 三角形绘制器



// 索引三角形获取处理器 
// 构造：存储索引的池
// 输入：索引值
// 输出：三角形
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

// 【顶点变换器】 
// 输入：顶点 变换矩阵
// 输出：变换后的顶点
// 顶点与矩阵的乘法 可用于顶点的世界变换、视觉变换、投影变换等
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

// 三角形列表绘制器
// 输入：顶点流 输出：像素
template <typename t_buffer>
struct ltrilist_drawer 
{
	t_buffer buffer;


	template <typename t_pool>
	bool operator()(t_pool vertexdata)
	{
		// 单流和多流的使用

		// 单流的问题：每个顶点结构要提供相应的处理器 顶点着色器 和像素着色器
		// 这种方法最简单 但渲染过程可控性较下

		// 第一阶段 顶点世界变换

		// 第二阶段 

		// 第二阶段 绘制三角形
	}
};


// 向量变换器
struct lvector_transformer
{
	lmatrix4f mat;

	float3 operator()(const float3& _vector)
	{
		return mat.trans_vector(_vector);
	}
};

// world_view的转换器
auto ltrans_world_view(const lmatrix4f& matworld, const lmatrix4f& matview)
{
	return lconnect(lvertex_transformer{ matworld }, lvertex_transformer{ matview });
}


// 【三角形裁剪器】
// 输入：待裁剪三角形 box裁剪体
// 输出：裁剪后的三角形 （0个或多个）
struct ltri_culler
{
	lboxf box;
	std::vector<ltri3df> operator()(const ltri3df& tir_in);
};

// 索引三角形处理器

