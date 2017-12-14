#pragma once

#include "lmath.hpp"

//////////////////////////////////////////////////////////////////////////
// ���ݴ�����
// ���ݴ�����ģ��Ĺ���ܼ򵥣�������operator()
// ���¶����˼����򵥵Ĵ�����

// �մ����� �����κε����ݴ�����ԭ������
struct lprocessor_null
{
	template <typename t>
	t operator () (const t& v)
	{
		return v;
	}

	template <typename t_in, typename t_out>
	t_out operator () (const t_in& v_in, t_out& v_out)
	{
		return v_out;
	}
};


// ���������� ���ܽ��ܵ�ʲôֵ����ֻ���س��������ֵ
template <typename t_data>
struct lprocessor_const
{
	t_data v;
	template <typename t>
	t_data operator () (const t&)
	{
		return v;
	}
};

// ����void�ĳ���������
typedef lprocessor_const<void> lprocessor_void;

// ���������� 
// ����������������  �����µĴ�����
// �����Ƕ���first�������second�ж��������
template<typename t_processor_first, typename t_processor_second>
struct lprocessor_connect
{
	t_processor_first processor_first;
	t_processor_second processor_second;

	template<typename t_data_in>
	inline auto operator()(const t_data_in& data_in)
	{
		return processor_second(processor_first(data_in));
	}

	template<typename t_in, typename t_out>
	inline auto operator()(const t_in& data_in, t_out& data_out)
	{
		return processor_second(processor_first(data_in), data_out);
	}
};

// TODO һ��������������������������������� ������������

// �򵥵���������������
template <typename t1, typename t2>
inline auto lconnect(t1&& pr1, t2&& pr2)
{
	return lprocessor_connect<t1, t2>{pr1, pr2};
}


