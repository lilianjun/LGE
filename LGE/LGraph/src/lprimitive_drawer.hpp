#pragma once

//////////////////////////////////////////////////////////////////////////
// 图元绘制器 串联一系列的流处理器


// 输入：描述图元的数据（如位置、法线、贴图等） 输出：光栅化后的图元（像素点集合）
// 构造：顶点流处理器（如世界变换） 图元处理器（如网格细分、三角形裁剪） 
//		顶点属性处理器（如光照、雾化） 图元光栅化器（如三角形光栅化）
// 内部将这些处理器进行了连接，用数据流打包、分包器和
template <typename t_primitive>
struct lprimitive_list_drawer
{
	// 缓存：中间数据的缓存

	// t_stream_in:输入的顶点数据
	// t_stream_out：输出的像素
	template <typename t_stream_in, typename t_stream_out>
	void operator()(const t_stream_in dat_in, t_stream_out& data_out)
	{
		
	}
};

// 索引的图元绘制器

// 【顶点处理流水线】
// 构造： t_vertex_processor 单个的顶点 处理器
// 输入：t_vertex_in数组 输入顶点数据流
// 输出：t_vertex_out数组 输出顶点数据流
// 例如：顶点变换阶段 将模型坐标变换到view空间坐标等
template <typename t_vertex_processor>
struct lvertex_process_pipeline
{
	t_vertex_processor vertex_processor;

	template<typename t_vertex_in, typename t_vertex_out, typename t_data_stream>
	bool operator ()(const t_data_stream<t_vertex_in>* pverin, t_data_stream<t_vertex_out>* pverout, uint32 count)
	{
		for (uint32 i : count)
		{
			vertex_processor(const t_vertex_in* pverin, t_vertex_out* pverout);
		}
		return true;
	}
};


//【图元处理流水线】 
// 构造： t_primitive_processor： 图元处理器 处理单个图元
// 输入： t_primitive_in *  输入图元数据流
// 输出： t_primitive_out *  输出图元数据流
// 如：三角形裁剪器等
template <typename t_primitive_processor>
struct lprimitive_process_pipeline
{
	t_primitive_processor primitive_processor;

	template <typename t_primitive_in, typename t_primitive_out>
	bool operator()(const t_primitive_in* ppriin, t_primitive_out* ppriout, uint32 count)
	{
		for (uint32 i : count)
		{
			const auto& trips = ptri[i];
			primitive_processor(const t_primitive_in* ppriin, t_primitive_out* ppriout);
		}
		return true;
	}
};


// 【顶点属性处理流水线】
// 可以看做vertex shader 在顶点阶段计算顶点属性，传入光栅化阶段
// 类似计算法线、光照等操作在这个阶段
// t_property_processor:单个单点属性的处理器
// t_pro_in：输入的属性 可以看做vs_in
// t_pro_out：输出的属性 可以看做vs_out
template <typename t_property_processor>
struct lproperty_process_pipeline
{
	t_property_processor prop_prc;

	template <typename t_pro_in, typename t_pro_out>
	bool operator()(const float3* pver, const t_pro_in* proin, const t_pro_out* proout, uint32 n)
	{
		for (uint32 i : n)
		{
			prop_prc(pver + i; proin + i; proout + i);
		}
		return true;
	}
};


// 【图元光栅化流水线】
template <typename t_primitive_raster>
struct lprimitive_raster_pipeline
{
	t_primitive_raster tri_drawer;

	template <typename t_pro_in>
	bool operator()(const float3* pver, const t_pro_in* pro, const lint3* ptri, uint32 n)
	{
		for (int i : n)
		{
			tri_drawer()
		}
	}
	return true;
};