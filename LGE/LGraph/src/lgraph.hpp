#pragma once

//#include "lmath.hpp"
#include <algorithm>
#include "lvectorn.h"
#include "lrender_triangle.h"
#include "lrender_line.h"

// ���������� 
// ����������������  �����µĴ�����
// �����Ƕ���first�������second�ж��������
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

// ĳ�ֽṹ���б�����
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
// ����βü��� nΪ����ĸ���������
// ���죺detector ��һ���㷨 ������������� �Լ����㽻��
// �����ص㣺
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
				// ���㶼���� ����p1
			}
			else if (bp1 && (!bp2))
			{
				// p1�ڣ� p2���� ����p1 �� ����zmax�Ľ���
				it = tri_list->emplace(it, intersection(*it, *it2));
			}
			else if ((!bp1) && bp2)
			{
				// p1���� p2���� �������
				//outp.push_back(tl(p1, p2).pz(pmax.z));
				it = tri_list->erase(it);
				tri_list->emplace(it, intersection(*it, *it2));
			}
			else if ((!bp1) && (!bp2))
			{
				// ���㶼���⣬ɾ��P1��
				it = tri_list->erase(it);
			}
		}
		return !(tri_list->size() < 3);
	}
};





