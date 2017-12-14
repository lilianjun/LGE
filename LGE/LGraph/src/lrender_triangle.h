#pragma once

// 渲染三角形算法
#include "lmath.hpp"
#include "lsoft_shader.h"
#include "lvertex_lerper.h"

// 渲染统计
struct  stRenderStatistics
{
	int ntri_total;
	int ntri_back_cull;
	int ntri_out_cull;
	int nver;
	int nmesh;
	int npixel;
	int npixel_depth_passed;

	void clear()
	{
		memset(this, 0, sizeof(stRenderStatistics));
	}
};

//////////////////////////////////////////////////////////////////////////
// 背面三角形裁剪器
// 构造：判断面法线与视线的方向
// 视线如何计算呢？取三点的中点，与原点
struct ltri_back_culler
{
	// 输入：三个顶点
	// 输出：是否是背面
	// 是否是背面
	inline bool operator()(const float* p1, const float* p2, const float* p3)
	{
		// 求三角形法线：
		// cross: (p2 - p1).nor, (p3 - p1).nor
		float2 v1{ p1[0], p1[1] };
		float2 v2{ p2[0], p2[1] };
		float2 v3{ p3[0], p3[1] };

		// 二维中，第三个点，在线的内侧或者外侧 用2dcoross计算
		auto p = (v2 - v1).cross(v3 - v1);
		return p > 0;
	}
};

//////////////////////////////////////////////////////////////////////////
// 多边形裁剪器 【与面的】
// 输入：点列表 点包含算法，交点算法
template <int nfloat>
struct lpologon_culler
{
	typedef lfloatn<nfloat> t_point;
	typedef std::list<t_point> t_point_list;

	template <typename t_isinside, typename t_intersection >
	inline bool operator()(t_point_list& point_list, t_isinside isindide, t_intersection intersection)
	{
		if (point_list.size() < 3) return 0;
		auto it = point_list.begin();
		auto it2 = it;

		t_point_list list_out;

		// 
		while ( it != point_list.end())
		{
			it2 = it;
			it2++;
			it2 = (it2 == point_list.end() ? point_list.begin() : it2);

			bool bp1 = isindide(*it);
			bool bp2 = isindide(*it2);

			if (bp1 && bp2)
			{
				// 两点都在内 输出p1
				list_out.push_back(*it);
				it++;
			}
			else if (bp1 && (!bp2))
			{

				// 输出p1和交点
				list_out.push_back(*it);
				list_out.emplace_back(intersection(*it, *it2));
				it++;
			}
			else if ((!bp1) && bp2)
			{
				// p1在外 p2在内 输出交点
				list_out.emplace_back(intersection(*it, *it2));
				it++;
			}
			else if ((!bp1) && (!bp2))
			{
				it++;
			}
		}
		point_list.swap(list_out);
		return (point_list.size() > 2);
	}

	template <typename t_isinside , typename t_intersection >
	inline bool operator()(t_point_list& point_list, t_isinside isindide, t_intersection intersection, int xx)
	{
		if (point_list.size() < 3) return 0;
		auto it = point_list.begin();
		auto it2 = it;
		
		t_point_list list_out;

		// 
		while (point_list.size() > 2 && it != point_list.end())
		{
			it2 = it;
			it2++;
			it2 = (it2 == point_list.end() ? point_list.begin() : it2);

			bool bp1 = isindide(*it);
			bool bp2 = isindide(*it2);

			if (bp1 && bp2)
			{
				// 两点都在内 保留p1 下一次从p2开始
				it++;
			}
			else if (bp1 && (!bp2))
			{
				
				// it2是第一个点的情况，把it2删了
				if (it2 == point_list.begin())
				{
					// 交点加在最后
					point_list.emplace(point_list.end(), intersection(*it, *it2));
					point_list.erase(it2);
				}
				else
				{
					// p1在， p2不在 保留p1 和 增加zmax的交点 交点增加在p1的后面
					// 下一步从P2开始
					it = point_list.emplace(it2, intersection(*it, *it2)); //
					it++;
				}
			}
			else if ((!bp1) && bp2)
			{
				// p1在外 p2在内 输出交点
				// 计算交点 删除p1点，p2前加入p1点 下一步从p2点开始 因为p1 p2都是在内的了
				//outp.push_back(tl(p1, p2).pz(pmax.z));
				auto ins = intersection(*it, *it2);
				if (it != point_list.begin())
				{
					it = point_list.erase(it); //
					it = point_list.insert(it, ins); // it指向交点
					it++;
				}
				else
				{
					// 是第一个点的情况， 加在第二个点前边
					it = point_list.insert(it2, ins);
					it++;
				}
				
			}
			else if ((!bp1) && (!bp2))
			{
				// p1 是起点的情况 就不删除p1，也不删p2
				if (it == point_list.begin() )
				{
					it++;
				}
				// p2是起点的情况，说明到终点了 就两个点都删除
				else if (it2 == point_list.begin())
				{
					point_list.erase(it);
					point_list.erase(it2);
					it = point_list.end();
				}
				else
				{
					// 两点都在外，删除P1点 下一次从p2点开始 it指向了p2点 不用++
					it = point_list.erase(it); // 
				}
			}
		}
	
		return (point_list.size() > 2);
	}
};

//////////////////////////////////////////////////////////////////////////
// 【三角形裁剪器】
// 构造：裁剪体box
// 输入：投影后的三个顶点 
// 输出：裁剪后的0~n个三角形
// 模板参数：nfloat 顶点的float数数量
template <int nfloat>
struct ltri_box_culler
{
	lboxf box;

	typedef lfloatn<nfloat> t_vertex;
	// 裁剪后的点的属性值也是需要插值的

	// 裁剪结果
	std::list<t_vertex> tri_list;

	void operator()(const float* p1, const float* p2, const float* p3)
	{
		tri_list.clear();

		// 背面裁剪
		if (!ltri_back_culler()(p1, p2, p3))
		{
			return;
		}

		// Z裁剪 一旦三角形有点在[0, 1]范围外，就直接抛弃此三角形 因为它太近或者太远。裁剪也没有意义
		if (p1[2] < box.pmin.z || p1[2] > box.pmax.z
			|| p2[2] < box.pmin.z || p2[2] > box.pmax.z
			|| p3[2] < box.pmin.z || p3[2] > box.pmax.z)
		{
			return;
		}

		tri_list = { lmake_floatn<nfloat>(p1), lmake_floatn<nfloat>(p2), lmake_floatn<nfloat>(p3) };

		lpologon_culler<nfloat> cull;

		// xmin裁剪 离开边界一个像素后再做裁剪
		auto cullminx = [this](const t_vertex& v) {return v[0] > box.pmin.x - 1.f;};
		auto interminx = [this](const t_vertex& v1, const t_vertex& v2)
		{ return llinen<float, nfloat>{v1, v2}.point(0, box.pmin.x); };
		if (!cull(tri_list, cullminx, interminx)) return;

		// x max裁剪
		auto cullmaxx = [this](const t_vertex& v) {return v[0] < box.pmax.x + 1.f;};
		auto intermaxx = [this](const t_vertex& v1, const t_vertex& v2)
		{ return llinen<float, nfloat>{v1, v2}.point(0, box.pmax.x); };
		if (!cull(tri_list, cullmaxx, intermaxx)) return;

		// y min 裁剪
		auto cullminy = [this](const t_vertex& v) {return v[1] > box.pmin.y - 1.f;};
		auto interminy = [this](const t_vertex& v1, const t_vertex& v2)
		{ return llinen<float, nfloat>{v1, v2}.point(1, box.pmin.y); };
		if (!cull(tri_list, cullminy, interminy)) return;

		// y max裁剪
		auto cullmaxy = [this](const t_vertex& v) {return v[1] < box.pmax.y + 1.f;};
		auto intermaxy = [this](const t_vertex& v1, const t_vertex& v2)
		{ return llinen<float, nfloat>{v1, v2}.point(1, box.pmax.y); };
		if (!cull(tri_list, cullmaxy, intermaxy)) return;

		// z裁剪
	}
};

//////////////////////////////////////////////////////////////////////////
// 屏幕空间三角形裁剪器
// 构造：rect
// 输入：三角形三个点
// 输出：裁剪后的0~n个三角形
template <int nfloat>
struct ltri_rect_culler
{
	lrectf rect;

	typedef lfloatn<nfloat> t_point;

	bool operator()(std::list<t_point>& tri_list)
	{
		lpologon_culler<nfloat> cull;

		// xmin裁剪 
		auto cullminx = [this](const t_point& v) {return !(v[0] < rect.pmin.x);};
		auto interminx = [this](const t_point& v1, const t_point& v2)
		{ return lpoint_on_line<float, nfloat>(v1, v2, 0, rect.pmin.x); };
		if (!cull(tri_list, cullminx, interminx)) return false;

		// x max裁剪
		auto cullmaxx = [this](const t_point& v) {return !(v[0] > rect.pmax.x );};
		auto intermaxx = [this](const t_point& v1, const t_point& v2)
		{ return lpoint_on_line<float, nfloat>(v1, v2, 0, rect.pmax.x); };
		if (!cull(tri_list, cullmaxx, intermaxx)) return false;

		// y min 裁剪
		auto cullminy = [this](const t_point& v) {return !(v[1] < rect.pmin.y);};
		auto interminy = [this](const t_point& v1, const t_point& v2)
		{ return lpoint_on_line<float, nfloat>(v1, v2, 1, rect.pmin.y); };
		if (!cull(tri_list, cullminy, interminy)) return false;

		// y max裁剪
		auto cullmaxy = [this](const t_point& v) {return !(v[1] > rect.pmax.y);};
		auto intermaxy = [this](const t_point& v1, const t_point& v2)
		{ return lpoint_on_line<float, nfloat>(v1, v2, 1, rect.pmax.y); };
		if (!cull(tri_list, cullmaxy, intermaxy)) return false;
		
		return true;
	}
};

//////////////////////////////////////////////////////////////////////////
// 三角形Z裁剪器
// 构造:neraZ farZ
// 输入：和near far同一空间的三角形
// 输出：裁剪后的三角形
template <size_t nfloat>
struct ltri_zculler
{
	float nearZ, farZ;

	typedef lfloatn<nfloat> t_point;

	bool operator()(std::list<lfloatn<nfloat> >& tri_list)
	{
		lpologon_culler<nfloat> cull;

		// zmin裁剪
		auto cullminz = [this](const t_point& v) {return !(v[2] < nearZ);};
		auto interminz = [this](const t_point& v1, const t_point& v2)
		{ return lpoint_on_line(v1, v2, 2, nearZ); };
		if (!cull(tri_list, cullminz, interminz)) return false;

		// zmax裁剪
		auto cullmaxz = [this](const t_point& v) {return !(v[2] > farZ);};
		auto intermaxz = [this](const t_point& v1, const t_point& v2)
		{ return lpoint_on_line(v1, v2, 2, farZ); };
		if (!cull(tri_list, cullmaxz, intermaxz)) return false;

		return true;
	}
};

//////////////////////////////////////////////////////////////////////////
// 【三角形光栅化器】
// 输入：投影后的三个顶点 代表三角形顶点
// 构造：像素处理程序
// 输出；由像素处理程序负责
template <const char* id_ps>
struct ltri_raster
{
	static stRenderStatistics* psta;

	using t_ps = ltps<id_ps>;
	using t_vertex = typename t_ps::t_in;
	t_ps ps;
	lcolor_buffer* clbuf;
	ldepth_buffer* dpbuf;

	bool bblend;

	inline void operator()(t_vertex* pin)
	{
		// 三个顶点 光栅化 插值属性
		// 然后对每个点用PS计算出颜色值
		this->operator()(pin, pin + 1, pin + 2);
	}

	inline void operator()(t_vertex* p1, t_vertex* p2, t_vertex* p3)
	{

		float* v1 = (float*)p1;
		float* v2 = (float*)p2;
		float* v3 = (float*)p3;

		// X Y，取整数
		v1[0] = lfround(v1[0]);
		v1[1] = lfround(v1[1]);
		v2[0] = lfround(v2[0]);
		v2[1] = lfround(v2[1]);
		v3[0] = lfround(v3[0]);
		v3[1] = lfround(v3[1]);

		// 重新排序 Y从小到大
		if (v1[1] > v2[1])
		{
			lswap(v1, v2);
		}
		if (v2[1] > v3[1])
		{
			lswap(v2, v3);
		}
		if (v1[1] > v2[1])
		{
			lswap(v1, v2);
		}

		const size_t _nflt = sizeof(t_vertex) / sizeof(float);
		lvertex_steperY<_nflt> l12, l13, l23;

		l12.beg(v1, v2);
		l13.beg(v1, v3);
		l23.beg(v2, v3);
		auto ptemp = &l12;
		while (!l13.end())
		{
			if (l12.end())
			{
				ptemp = &l23;
			}
			auto pL = ptemp->cur.data();
			auto pR = l13.cur.data();
			// 保证Y相同且为整数
			//pf2L[1] = ptemp->ai;
			//pf2R[1] = l13.ai;
			assert( lequal( pL[1],  pR[1]) );
			if (pL[0] > pR[0])
			{
				lswap(pL, pR);
			}
			lvertex_steperX<_nflt> llr;
			// 把X也切换为整数
			llr.beg(pL, pR);
			llr.p1[0] = lfround(llr.p1[0]);
			llr.p2[0] = lfround(llr.p2[0]);
			
			while (!llr.end())
			{
				// 点就是llr.bi
				const float3* pos = (float3*)llr.cur.data();
				int x = liround(pos->x);
				int y = liround(pos->y);
				psta->npixel++;

				if (y < 10)
				{
					x += 1;
				}

				// 深度测试
				if (dpbuf->test(x, y, pos->z))
				{
					auto cl = ps.process(*((typename t_ps::t_in*)llr.cur.data()));
					
					if (bblend)
					{
						// 透明度测试
						if (cl.A >  0.0001f)
						{
							lcolor cl2 = lcolor::create(clbuf->get(x, y));
							cl2.f4.xyz() = cl2.f4.xyz().lerp(cl.f4.xyz(), 1.f - cl.A);
							clbuf->set(x, y, cl2.get_argb());
							dpbuf->set(x, y, pos->z);
						}
						
					}
					else
					{
						clbuf->set(x, y, cl.get_argb());
						dpbuf->set(x, y, pos->z);
					}
				}
				else
				{
					psta->npixel_depth_passed++;
				}
				llr.next();
			}

			// 中间一行画两次 13不增加
			ptemp->next();
			if ( ptemp == &l12 )
			{
				if(!l12.end())
					l13.next();
			}
			else
				l13.next();
			
		}
	}
};

template <const char* id_ps>
stRenderStatistics* ltri_raster<id_ps>::psta = nullptr;



//////////////////////////////////////////////////////////////////////////
// 【三角形列表渲染器】
// 构造：顶点程序和像素程序
// 输入：顶点数据
// 输出：会在像素程序中执行输出
template <const char* id_vs = lvs_default, const char* id_ps = lps_default>
struct lttri_list_render
{
	static stRenderStatistics* psta;

	// 构造：顶点着色器 像素着色器 投影器
	using t_vs = ltvs<id_vs>;
	using t_ps = ltps<id_ps>;

	typedef typename t_vs::t_out t_vsout;

	bool bdep_test;
	bool bdep_write;
	bool bback_cull;
	// 是否开启透明混合
	bool bblend;

	lttri_list_render():bdep_test(true), bdep_write(true), bback_cull(true), bblend(true) {}

	lttri_list_render(lcolor_buffer* _clbuf, ldepth_buffer* _dpbuf)
		:bdep_test(true), bdep_write(true), bback_cull(true), bblend(true)
	{
		raster.clbuf = _clbuf;
		raster.dpbuf = _dpbuf;
	}

	// 获取着色器
	t_vs* getvs() { return &vs; }
	t_ps* getps() { return &raster.ps; }

	void setbuf(lcolor_buffer* _clbuf, ldepth_buffer* _dpbuf)
	{
		raster.clbuf = _clbuf;
		raster.dpbuf = _dpbuf;
	}

	// 渲染操作 输入：顶点数据 索引数据
	void render(const typename t_vs::t_in* pvsin, int nver, const lint3* pindex, int ntri)
	{
		psta->nmesh += 1;
		psta->nver += nver;
		psta->ntri_total += ntri;
		raster.psta = psta;
		raster.bblend = bblend;

		// 【顶点程序阶段】
		vsout.resize(nver);
		for (int i = 0; i < nver; i++)
		{
			vsout[i] = vs.process(pvsin[i]);
		}
		t_vsout* prin = vsout.data();
		
		// 光栅化阶段
		const auto nflt = sizeof(t_vsout) / sizeof(float);

		ltri_zculler<nflt> zcull{vs.proj_trans->zn, vs.proj_trans->zf};

		ltri_rect_culler<nflt> rectcull{ lrectf{0, 0, vs.proj_trans->w_vp, vs.proj_trans->h_vp}  };

		ltri_back_culler backcull;

		for (int i = 0; i < ntri; i++)
		{

			auto p1 = prin + pindex[i].x;
			auto p2 = prin + pindex[i].y;
			auto p3 = prin + pindex[i].z;

			std::list<lfloatn<nflt>> tri_list{ lmake_floatn<nflt>((const float*)p1),
				lmake_floatn<nflt>((const float*)p2), lmake_floatn<nflt>((const float*)p3) };

			// TODO:背面裁剪可放于这个阶段

			// Z裁剪
			if (!zcull(tri_list))
			{
				psta->ntri_out_cull += 1;
				continue;
			}

			// 投影三角形顶点
			for (auto& point : tri_list)
			{
				float3* pv = (float3*)&point;
				*pv =  vs.proj_trans->trans(*pv);
			}

			// 屏幕裁剪
			if (!rectcull(tri_list))
			{
				psta->ntri_out_cull += 1;
				continue;
			}

			// 光栅化
			// 从裁剪结果里取出所有的三角形进行绘制
			auto it1 = tri_list.begin();
			auto it2 = it1; it2++;
			auto it3 = it2; it3++;

			while (it3 != tri_list.end())
			{
				// 背面裁剪
				if ( !bback_cull || ( bback_cull && backcull(it1->data(), it2->data(), it3->data())))
				{
					raster((t_vsout*)&*it1, (t_vsout*)&*it2, (t_vsout*)&*it3);
					it2 = it3;
				}
				else
				{
					psta->ntri_back_cull += 1;
				}
				
				it3++;
			}
		}
	}

private:
	// 顶点程序
	t_vs vs;
	// 暂存中间数据
	std::vector<t_vsout> vsout;
	// 三角形光栅化器
	ltri_raster<id_ps> raster;
};

 template <const char* id_vs /*= lvs_default*/, const char* id_ps /*= lps_default*/>
stRenderStatistics* lttri_list_render<id_vs, id_ps>::psta = nullptr;
