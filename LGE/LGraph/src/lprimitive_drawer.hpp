#pragma once

//////////////////////////////////////////////////////////////////////////
// ͼԪ������ ����һϵ�е���������


// ���룺����ͼԪ�����ݣ���λ�á����ߡ���ͼ�ȣ� �������դ�����ͼԪ�����ص㼯�ϣ�
// ���죺��������������������任�� ͼԪ��������������ϸ�֡������βü��� 
//		�������Դ�����������ա����� ͼԪ��դ�������������ι�դ����
// �ڲ�����Щ���������������ӣ���������������ְ�����
template <typename t_primitive>
struct lprimitive_list_drawer
{
	// ���棺�м����ݵĻ���

	// t_stream_in:����Ķ�������
	// t_stream_out�����������
	template <typename t_stream_in, typename t_stream_out>
	void operator()(const t_stream_in dat_in, t_stream_out& data_out)
	{
		
	}
};

// ������ͼԪ������

// �����㴦����ˮ�ߡ�
// ���죺 t_vertex_processor �����Ķ��� ������
// ���룺t_vertex_in���� ���붥��������
// �����t_vertex_out���� �������������
// ���磺����任�׶� ��ģ������任��view�ռ������
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


//��ͼԪ������ˮ�ߡ� 
// ���죺 t_primitive_processor�� ͼԪ������ ������ͼԪ
// ���룺 t_primitive_in *  ����ͼԪ������
// ����� t_primitive_out *  ���ͼԪ������
// �磺�����βü�����
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


// ���������Դ�����ˮ�ߡ�
// ���Կ���vertex shader �ڶ���׶μ��㶥�����ԣ������դ���׶�
// ���Ƽ��㷨�ߡ����յȲ���������׶�
// t_property_processor:�����������ԵĴ�����
// t_pro_in����������� ���Կ���vs_in
// t_pro_out����������� ���Կ���vs_out
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


// ��ͼԪ��դ����ˮ�ߡ�
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