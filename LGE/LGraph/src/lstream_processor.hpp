#pragma once

#include <memory>
#include <vector>
#include <list>
#include "ldata_processor.hpp"

//////////////////////////////////////////////////////////////////////////
// �������� �������ݵ���������
// �������������������
// ��������Ҳ������connect�������� ������� ���ݳ� ��������������

template <typename t>
inline auto lmake_vector(initializer_list<t>&& vs)
{
	return std::make_shared<std::vector<t>>(vs);
}

template <typename t>
inline auto lmake_list(initializer_list<t>&& vs)
{
	return std::make_shared<std::list<t> >(vs);
}

template <typename t>
auto lmake_array(initializer_list<t>&& vs)
{
	return std::make_shared<std::array<t> >(vs);
}

// ��������������
template <typename t_stream_in, typename t_stream_out>
struct  lstream_processor_null
{
	t_stream_out operator()(t_stream_in&& data_in)
	{
		return t_stream_out{};
	}
};

// cpu forѭ��˳����������
template <typename t_stream_out, typename t_processor>
struct lstream_processor
{
	t_stream_out pool_out;
	t_processor processor;

	template <typename t_pool_int, >
	t_stream_out operator()(t_stream_in pool_in)
	{
		std::transform(pool_in->beg(), pool_in->end(), pool_out->begin(), processor);
		return pool_out;
	}

};

template < typename t_stream_out, typename t_processor>
inline auto lmake_processor(t_stream_out pool, t_processor processor)
{
	return lstream_processor<t_stream_out, t_processor>{pool, processor};
}


template < typename t_stream_out, typename t_processor, typename t_getter>
inline auto lmake_processor(t_stream_out pool, t_processor processor, t_getter geter)
{
	return lstream_processor_getter<t_stream_out, t_processor, t_getter>{pool, processor, getter};
}

//////////////////////////////////////////////////////////////////////////
// ����ֵ��ȡ������
// ���룺����ֵ ��
// �������������ֵ����Ӧ��ֵ
template <typename t_pool>
struct lindex_getter
{
	t_pool pool;
	auto operator()(int i)
	{
		return *(pool.beg() + i);
	}
};






// ����������getter setter��
// ���������δ������Ķ�������:
// �ı䴦����  ltri processor()(index_pool )
void ff()
{
	auto ver = lmake_vector<float3>();
	auto tri = lmake_list<lint3>();
	
}

// �������������� ����һ����������ֵ������������һ������
struct llindex_stream_processor 
{
	
};
