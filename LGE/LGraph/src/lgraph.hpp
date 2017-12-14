#pragma once

//#include "lmath.hpp"
#include <algorithm>
#include "lvectorn.h"
#include "lrender_triangle.h"
#include "lrender_line.h"

// 【连接器】 
// 将两个处理器连接  构造新的处理器
// 条件是对于first的输出，second有对象的输入
template<typename t_processor_first, typename t_processor_second>
struct lprocessor_connect
{
	t_processor_first processor_first;
	t_processor_second processor_second;

	void operator()()
	{
		processor_first();
		processor_second();
	}

	template<typename t_data_in>
	inline auto operator()(const t_data_in& data_in)
	{
		return processor_second(processor_first(data_in));
	}
};

// 某种结构的列表处理器
struct lvertex_list_porcessor
{
	//
	template < typename t_vsin, typename t_vsout, typename t_vs>
	void operator()(const t_vsin* pin, int n, t_vsout* pout, t_vs vs)
	{
		std::transform(pin, pin + n, pout, vs);
	}
};




//////////////////////////////////////////////////////////////////////////
// 多边形裁剪器 n为顶点的浮点数数量
// 构造：detector 是一个算法 计算点在面内外 以及计算交点
// 两个重点：
template <int nfloat >
struct lpolygon_culler
{
	typedef lfloatn<nfloat> t_vertex;

	std::list<t_vertex>* tri_list;

	template < typename t_isinside, typename t_intersection >
	int operator()(t_isinside&& isindide, t_intersection intersection)
	{
		if (tri_list->size() < 3) return 0;

		for (auto it = tri_list->begin(); it != tri_list->end(); it++)
		{
			auto nextit = it; nextit++;
			auto it2 = (nextit == tri_list->end() ? tri_list->begin() : nextit);
			bool bp1 = isindide(*it);
			bool bp2 = isindide(*it2);
			if (bp1 && bp2)
			{
				// 两点都在内 保留p1
			}
			else if (bp1 && (!bp2))
			{
				// p1在， p2不在 保留p1 和 增加zmax的交点
				it = tri_list->emplace(it, intersection(*it, *it2));
			}
			else if ((!bp1) && bp2)
			{
				// p1在外 p2在内 输出交点
				//outp.push_back(tl(p1, p2).pz(pmax.z));
				it = tri_list->erase(it);
				tri_list->emplace(it, intersection(*it, *it2));
			}
			else if ((!bp1) && (!bp2))
			{
				// 两点都在外，删除P1点
				it = tri_list->erase(it);
			}
		}
		return !(tri_list->size() < 3);
	}
};





