#pragma once

#include "lmath.hpp"

//////////////////////////////////////////////////////////////////////////
// 【线的屏幕空间的rect裁剪】
template <size_t nflt>
struct lline_rect_culler
{
	lrectf rect;
	lfloatn<nflt> outp1, outp2;

	lline_rect_culler() = default;

	lline_rect_culler(const lrectf& _r)
		:rect(_r)
	{ }

	// 输入：线的两个点 输出：裁剪后的线或者空
	// 返回值：0:线被裁剪掉了 1线的结果保存在outp1 outp2中
	bool operator()(const float* _p1, const float* _p2)
	{
		// 判断线的两个点，都在内，一个点在内，一个点在外
		p1 = (float3*)_p1;
		p2 = (float3*)_p2;

		// 两点距离小于1就抛弃
		//if (p1->xy().distance(p2->xy()) < 1.f)
		//	return 0;

		outp1 = lmake_floatn<nflt>(_p1);
		outp2 = lmake_floatn<nflt>(_p2);

		int ret = 0;
		if (!_xmin())return 0;
		if (!_xmax())return 0;
		if (!_ymin())return 0;
		if (!_ymax())return 0;
		return 1;
		
	}

private:
	const float3* p1;
	const float3* p2;

	bool _xmin()
	{
		// zmin 裁剪
		bool b1 = p1->x > rect.pmin.x - 0.00001f;
		bool b2 = p2->x > rect.pmin.x - 0.00001f;
		if (b1 && b2)
			return true;
		else if ((!b1) && (!b2))
			return false;
		else if (b1 && (!b2))
		{
			// p1 和交点
			llinen<float, nflt> myline{ outp1, outp2 };
			outp2 = myline.point(0, rect.pmin.x);
			return true;
		}
		else if ((!b1) && (b2))
		{
			// 交点和p2
			llinen<float, nflt> myline{ outp1, outp2 };
			outp1 = myline.point(0, rect.pmin.x);
			return true;
		}
		assert(0); return false;
	}

	bool _xmax()
	{
		// zmin 裁剪
		bool b1 = p1->x < rect.pmax.x + 0.00001f;
		bool b2 = p2->x < rect.pmax.x + 0.00001f;
		if (b1 && b2)
			return true;
		else if ((!b1) && (!b2))
			return false;
		else if (b1 && (!b2))
		{
			// p1 和交点
			llinen<float, nflt> myline{ outp1, outp2 };
			outp2 = myline.point(0, rect.pmax.x);
			return true;
		}
		else if ((!b1) && (b2))
		{
			// 交点和p2
			llinen<float, nflt> myline{ outp1, outp2 };
			outp1 = myline.point(0, rect.pmax.x);
			return true;
		}
		assert(0); return false;

	}

	bool _ymin()
	{
		// zmin 裁剪
		bool b1 = p1->y > rect.pmin.y - 0.00001f;
		bool b2 = p2->y > rect.pmin.y - 0.00001f;
		if (b1 && b2)
			return true;
		else if ((!b1) && (!b2))
			return false;
		else if (b1 && (!b2))
		{
			// p1 和交点
			llinen<float, nflt> myline{ outp1, outp2 };
			outp2 = myline.point(1, rect.pmin.y);
			return true;
		}
		else if ((!b1) && (b2))
		{
			// 交点和p2
			llinen<float, nflt> myline{ outp1, outp2 };
			outp1 = myline.point(1, rect.pmin.y);
			return true;
		}
		assert(0); return false;
	}

	bool _ymax()
	{
		// zmin 裁剪
		bool b1 = p1->y < rect.pmax.y + 0.00001f;
		bool b2 = p2->y < rect.pmax.y + 0.00001f;
		if (b1 && b2)
			return true;
		else if ((!b1) && (!b2))
			return false;
		else if (b1 && (!b2))
		{
			// p1 和交点
			llinen<float, nflt> myline{ outp1, outp2 };
			outp2 = myline.point(1, rect.pmax.y);
			return true;
		}
		else if ((!b1) && (b2))
		{
			// 交点和p2
			llinen<float, nflt> myline{ outp1, outp2 };
			outp1 = myline.point(1, rect.pmax.y);
			return true;
		}
		assert(0); return false;

	}
};

//////////////////////////////////////////////////////////////////////////
// 线的Z裁剪
// 构造:neraZ farZ
// 输入：和near far同一空间的线
// 输出：裁剪后的线
template <size_t nflt>
struct lline_zculler
{
	float nearZ, farZ;
	lfloatn<nflt> outp1, outp2;
	
	bool operator()(const float* _p1, const float* _p2)
	{
		float3* p1 = (float3*)_p1;
		float3* p2 = (float3*)_p2;
		outp1 = lmake_floatn<nflt>(_p1);
		outp2 = lmake_floatn<nflt>(_p2);

		bool b1 = p1->z > nearZ - 0.00001f;
		bool b2 = p2->z > nearZ - 0.00001f;
		bool ret = false;
		if (b1 && b2)
			ret = true;
		else if ((!b1) && (!b2))
			return false;
		else if (b1 && (!b2))
		{
			// p1 和交点
			llinen<float, nflt> myline{ outp1, outp2 };
			outp2 = myline.point(2, nearZ);
			ret = true;
		}
		else if ((!b1) && (b2))
		{
			// 交点和p2
			llinen<float, nflt> myline{ outp1, outp2 };
			outp1 = myline.point(2, nearZ);
			ret = true;
		}

		b1 = p1->z < farZ + 0.00001f;
		b2 = p2->z < farZ + 0.00001f;
		if (b1 && b2)
			ret = true;
		else if ((!b1) && (!b2))
			return false ;
		else if (b1 && (!b2))
		{
			// p1 和交点
			llinen<float, nflt> myline{ outp1, outp2 };
			outp2 = myline.point(2, farZ);
			ret = true;
		}
		else if ((!b1) && (b2))
		{
			// 交点和p2
			llinen<float, nflt> myline{ outp1, outp2 };
			outp1 = myline.point(2, farZ);
			ret = true;
		}
		return ret;
	}
};

//////////////////////////////////////////////////////////////////////////
// 【线光栅化器】
template <typename t_ps>
struct lline_raster
{
	t_ps ps;
	typedef typename t_ps::t_in t_psin;

	void operator()(const t_psin* p1, const t_psin* p2)
	{
		// 进行裁剪
		const size_t _nflt = sizeof(t_psin) / sizeof(float);
		using t_steper = llinen_steper<float, _nflt>;
		// 选择X还是Y
		t_steper line((float*)p1, (float*)p2);
		line.p1[0] = lfround(line.p1[0]);
		line.p1[1] = lfround(line.p1[1]);
		line.p2[0] = lfround(line.p2[0]);
		line.p2[1] = lfround(line.p2[1]);
		abs(line.p1[0] - line.p2[0]) > abs(line.p1[1] - line.p2[1]) ? line.beg(0) : line.beg(1);

		while (!line.end())
		{
			ps(*(t_psin*)(line.cur.data()));
			line.next();
		}
	}
};

//////////////////////////////////////////////////////////////////////////
// 画线
template <typename t_vs, typename t_ps, typename t_proj>
struct lrender_line_list
{
	t_vs vs;
	t_ps ps;
	t_proj proj;
	
	// 缓存
	std::vector<typename t_vs::t_out> vsout;
	typedef typename t_vs::t_out t_vsout;
	typedef typename t_vs::t_in t_vsin;

	void operator()(const t_vsin* vers, int nline)
	{
		const size_t _nflt = sizeof(t_vsout) / sizeof(float);
		lline_zculler<_nflt> zculler{ proj.n, proj.f };
		lline_rect_culler<_nflt> rectculler{ lrectf(0, 0, proj.bufW, proj.bufH) };
		for (int i = 0; i < nline; i++)
		{
			// 顶点着色器
			auto p1 = vs(vers[i * 2]);
			auto p2 = vs(vers[i * 2 + 1]);

			// view空间z裁剪 
			bool bzcull = zculler((float*)&p1, (float*)&p2);

			if (!bzcull)continue;

			// 投影
			proj(zculler.outp1.data());
			proj(zculler.outp2.data());

			// 屏幕空间x、y裁剪
			bool brectcull = rectculler(zculler.outp1.data(), zculler.outp2.data());
			if (!brectcull)continue;

			// 进行光栅化
			lline_raster<t_ps>{ps}((t_vsout*)rectculler.outp1.data(), (t_vsout*)rectculler.outp2.data());
			continue;
			

		}
	}
};
