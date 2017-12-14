#pragma once

#include "lvector3.h"
#include "lshape2d.h"
#include <vector>

//////////////////////////////////////////////////////////////////////////
// 提前声明
// 球形
// 3d的轴向长方形
template <typename tv>
class labox;
// 3d的任意长方体
template <typename tv>
class lbox;
template <typename tv>
class lsphere;
// 3d的射线
template <typename tv>
class lray3d;
// 3d的线段
template <typename tv>
class lline3d;
// 3d的三角形
template <typename tv>
class ltri3d;
	// 平面
template <typename tv>
class lplane;
	// 视锥体
template <typename tv>
class lfrustum;

// 3d的轴向长方形
template <typename tv>
class labox
{
public:

};

// 求解直线方程

// 3d的任意长方体
template <typename tv>
class lbox
{
public:
	typedef lvector3<tv> tv3;
	typedef lline3d<tv> tl;

	tv3 pmin, pmax;

	lbox (const tv3& _pmin, const tv3& _pmax)
		:pmin(_pmin), pmax(_pmax)
	{ }

	bool inside(const tv3& p)
	{
		return inside(p.x, pmin.x, pmax.x) &&
			inside(p.y, pmin.y, pmax.y) &&
			inside(p.z, pmin.z, pmax.z);
	}

	// 求得直线与box的交点 最多两个?
	int intersection(const lline3d<tv>& line)
	{

	}

	// 裁剪直线 返回是否在内
	bool cull(const lline3d<tv>& line, lline3d<tv> line_out)
	{
		// 只考虑xy平面

	}

	// 线与box的交点
	tv3 intersection(const tv3& p1, const tv3& p2);

	// 六个平面进行裁剪

	// 裁剪多边形
	inline std::vector<tv3> cullxmin(const std::vector<tv3>& in_p)const
	{
		if (in_p.size() < 2)return in_p;

		std::vector<tv3> outp;
		for (int i = 0; i < in_p.size(); i++)
		{
			int i2 = (i + 1 == in_p.size() ? 0 : i + 1);
			const auto p1 = in_p[i];
			const auto p2 = in_p[i2];
			bool bp1 = p1.x > pmin.x;
			bool bp2 = p2.x > pmin.x;
			if (bp1 && bp2)
			{
				// 两点都在内 输出p1
				outp.push_back(p1);
			}
			else if (bp1 && (!bp2))
			{
				// p1在， p2不在 输出p1 和 xmin的交点
				outp.push_back(p1);
				outp.push_back(tl(p1, p2).px(pmin.x));
			}
			else if ((!bp1) && bp2)
			{
				// p1在外 p2在内 输出交点
				outp.push_back(tl(p1, p2).px(pmin.x));
			}
		}
		return outp;
	}

	inline std::vector<tv3> cullxmax(const std::vector<tv3>& in_p)const
	{
		
		if (in_p.size() < 2)return in_p;

		std::vector<tv3> outp;
		for (int i = 0; i < in_p.size(); i++)
		{
			int i2 = (i + 1 == in_p.size() ? 0 : i + 1);
			const auto p1 = in_p[i];
			const auto p2 = in_p[i2];
			bool bp1 = p1.x < pmax.x;
			bool bp2 = p2.x < pmax.x;
			if (bp1 && bp2)
			{
				// 两点都在内 输出p1
				outp.push_back(p1);
			}
			else if (bp1 && (!bp2))
			{
				// p1在， p2不在 输出p1 和 xmax的交点
				outp.push_back(p1);
				outp.push_back(tl(p1, p2).px(pmax.x));
			}
			else if ((!bp1) && bp2)
			{
				// p1在外 p2在内 输出交点
				outp.push_back(tl(p1, p2).px(pmax.x));
			}
		}
		return outp;
	}

	inline std::vector<tv3> cullymin(const std::vector<tv3>& in_p)const
	{
		if (in_p.size() < 2)return in_p;

		std::vector<tv3> outp;
		for (int i = 0; i < in_p.size(); i++)
		{
			int i2 = (i + 1 == in_p.size() ? 0 : i + 1);
			const auto p1 = in_p[i];
			const auto p2 = in_p[i2];
			bool bp1 = p1.y > pmin.y;
			bool bp2 = p2.y > pmin.y;
			if (bp1 && bp2)
			{
				// 两点都在内 输出p1
				outp.push_back(p1);
			}
			else if (bp1 && (!bp2))
			{
				// p1在， p2不在 输出p1 和 ymin的交点
				outp.push_back(p1);
				outp.push_back(tl(p1, p2).py(pmin.y));
			}
			else if ((!bp1) && bp2)
			{
				// p1在外 p2在内 输出交点
				outp.push_back(tl(p1, p2).py(pmin.y));
			}
		}
		return outp;
	}

	inline std::vector<tv3> cullymax(const std::vector<tv3>& in_p)const
	{
		if (in_p.size() < 2)return in_p;

		std::vector<tv3> outp;
		for (int i = 0; i < in_p.size(); i++)
		{
			int i2 = (i + 1 == in_p.size() ? 0 : i + 1);
			const auto p1 = in_p[i];
			const auto p2 = in_p[i2];
			bool bp1 = p1.y < pmax.y;
			bool bp2 = p2.y < pmax.y;
			if (bp1 && bp2)
			{
				// 两点都在内 输出p1
				outp.push_back(p1);
			}
			else if (bp1 && (!bp2))
			{
				// p1在， p2不在 输出p1 和 ymax的交点
				outp.push_back(p1);
				outp.push_back(tl(p1, p2).py(pmax.y));

			}
			else if ((!bp1) && bp2)
			{
				// p1在外 p2在内 输出交点
				outp.push_back(tl(p1, p2).py(pmax.y));
			}
		}
		return outp;
	}

	inline std::vector<tv3> cullzmin(const std::vector<tv3>& in_p)const
	{
		std::vector<tv3> outp;
		if (in_p.size() < 2)return in_p;

		for (int i = 0; i < in_p.size(); i++)
		{
			int i2 = (i + 1 == in_p.size() ? 0 : i + 1);
			const auto p1 = in_p[i];
			const auto p2 = in_p[i2];
			bool bp1 = p1.z > pmin.z;
			bool bp2 = p2.z > pmin.z;
			if (bp1 && bp2)
			{
				// 两点都在内 输出p1
				outp.push_back(p1);
			}
			else if (bp1 && (!bp2))
			{
				// p1在， p2不在 输出p1 和 zmin的交点
				outp.push_back(p1);
				outp.push_back(tl(p1, p2).pz(pmin.z));
			}
			else if ((!bp1) && bp2)
			{
				// p1在外 p2在内 输出交点
				outp.push_back(tl(p1, p2).pz(pmin.z));
			}
		}
		return outp;
	}

	inline std::vector<tv3> cullzmax(const std::vector<tv3>& in_p)const
	{
		std::vector<tv3> outp;
		if (in_p.size() < 2) return in_p;

		for (int i = 0; i < in_p.size(); i++)
		{
			int i2 = (i + 1 == in_p.size() ? 0 : i + 1);
			const auto p1 = in_p[i];
			const auto p2 = in_p[i2];
			bool bp1 = p1.z < pmax.z;
			bool bp2 = p2.z < pmax.z;
			if (bp1 && bp2)
			{
				// 两点都在内 输出p1
				outp.push_back(p1);
			}
			else if (bp1 && (!bp2))
			{
				// p1在， p2不在 输出p1 和 zmax的交点
				outp.push_back(p1);
				outp.push_back(tl(p1, p2).pz(pmax.z));
			}
			else if ((!bp1) && bp2)
			{
				// p1在外 p2在内 输出交点
				outp.push_back(tl(p1, p2).pz(pmax.z));
			}
		}
		return outp;
	}

	inline std::vector<tv3> cullx(const std::vector<tv3>& in_p)const
	{
		return cullxmin(cullxmax(in_p));
	}

	inline std::vector<tv3> cully(const std::vector<tv3>& in_p)const
	{
		return cullymin(cullymax(in_p));
	}

	inline std::vector<tv3> cullz(const std::vector<tv3>& in_p)const
	{
		return cullzmin(cullzmax(in_p));
	}

	// 裁剪多边形
	std::vector<tv3> cull(const std::vector<tv3>& in_p)const
	{
		return cullx(cully(cullz(in_p)));
	}

	// 裁剪三角形
	std::vector<ltri3d<tv> > cull(const ltri3d<tv>& tri)const
	{
		std::vector<ltri3d<tv> > trivec;
		auto ps = cull(std::vector<tv3>{tri.p1, tri.p2, tri.p3});
		if (ps.size() < 3) return trivec;
		for (size_t i = 1; i < ps.size() - 1; i ++)
		{
			trivec.push_back(ltri3d<tv>{ ps[0], ps[i], ps[i + 1]});
		}
		return trivec;
	}
};

// 球形
template <typename tv>
class lsphere
{

};



// 3d的射线
template <typename tv>
class lray3d
{
public:
	
};


template <typename tv>
class lline3d
{
public:
	typedef lvector3<tv> tv3;

	lline3d() {}

	lline3d(const tv3& _p1, const tv3 _p2)
		:p1(_p1), p2(_p2)
	{ }

	tv3 p1, p2;

	// 返回2D线
	lline2d<tv> linexy()const
	{
		return lline2d<tv>(p1.xy(), p2.xy());
	}

	lline2d<tv> linexz()const
	{
		return lline2d<tv>(p1.xz(), p2.xz());
	}

	lline2d<tv> lineyz()const
	{
		return lline2d<tv>(p1.yz(), p2.yz());
	}

	// 已知x ，求直线上的点
	tv3 px(tv _x)
	{
		auto w = lweight(p1.x, p2.x, _x);
		return tv3(_x,
			llerp(p1.y, p2.y, w),
			llerp(p1.z, p2.z, w));
	}

	// 已知y ，求直线上的点
	tv3 py(tv _y)
	{
		auto w = lweight(p1.y, p2.y, _y);
		return tv3(llerp(p1.x, p2.x, w),
			_y,
			llerp(p1.z, p2.z, w));
	}

	// 已知z ，求直线上的点
	tv3 pz(tv _z)
	{
		auto w = lweight(p1.z, p2.z, _z);
		return tv3( llerp(p1.x, p2.x, w),
			llerp(p1.y, p2.y, w),
			_z);
	}
};


// 3d的三角形
template <typename tv>
class ltri3d
{
public:
	typedef lvector3<tv> tv3;
	tv3 p1, p2, p3;

	lline3d<tv> line1()
	{
		return lline3d<tv>(p1, p2);
	}

	lline3d<tv> line2()
	{
		return lline3d<tv>(p1, p3);
	}

	lline3d<tv> line3()
	{
		return lline3d<tv>(p3, p1);
	}
};


// 平面
template <typename tv>
struct lplane
{
	typedef lvector3<tv> t_point;
	typedef lline3d<tv> t_line;
	t_point p1, p2, p3;

	// 三个点构造一个平面

	// 三角形

	// 直线作为法线，和法线上的一个点，狗仔一个平面
};


// 视锥体
template <typename tv>
class lfrustum
{
public:
	// 与三角形的碰撞检测
	bool detect(const ltri3d<tv>& tri);
};