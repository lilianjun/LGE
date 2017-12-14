#pragma once

#include "lmath.hpp"

//////////////////////////////////////////////////////////////////////////
// 数据处理器
// 数据处理器模板的规则很简单，定义了operator()
// 如下定义了几个简单的处理器

// 空处理器 不做任何的数据处理，按原样返回
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


// 常量处理器 不管接受到什么值，都只返回常量构造的值
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

// 返回void的常量处理器
typedef lprocessor_const<void> lprocessor_void;

// 【连接器】 
// 将两个处理器连接  构造新的处理器
// 条件是对于first的输出，second有对象的输入
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

// TODO 一个对象的输入来自于另外两个对象的输出 有两个参数的

// 简单的连接器创建方法
template <typename t1, typename t2>
inline auto lconnect(t1&& pr1, t2&& pr2)
{
	return lprocessor_connect<t1, t2>{pr1, pr2};
}


