#pragma once

#include "lvector2.h"

template <typename tv>
class lline2d;

template <typename tv>
class lrect;

template <typename tv>
class ltri2d;

enum EDetect
{
	// ���� ���� ����
	eOutSide, eInSide, eCross
};

// һ�����Ƿ���������֮��
template <typename tv>
bool inside(const tv& vdet, const tv& vmin, const tv& vmax)
{
	return  lgreeq(vdet, vmin) && llesseq(vdet, vmax);
}

// 2d���߶�
template <typename tv>
class lline2d
{
public:
	lline2d() = default;

	typedef lvector2<tv> tv2;

	lline2d(const lvector2<tv>& _p1, const lvector2<tv>& _p2)
		: p1(_p1), p2(_p2)
	{

	}

	lvector2<tv> p1, p2;

	tv len()const
	{
		return p1.distance(p2);
	}


	// ��� ��֪X
	tv2 px(tv _x)
	{
		return tv2(_x, llerp(p1.y, p2.y, lweight(p1.x, p2.x, _x)));
	}

	// ��� ��֪Y
	tv2 py(tv _y)
	{
		return tv2(llerp(p1.x, p2.y, lweight(p1.y, p2.y, _y)), _y);
	}
};

// 2d��������
template <typename tv>
class ltri2d
{
public:
	lvector2<tv> p1, p2, p3;

	ltri2d() = default;

	ltri2d(const lvector2<tv>& _p1, const lvector2<tv>& _p2, const lvector2<tv>& _p3 )
	: p1(_p1), p2(_p2), p3(_p3)
	{ }

	lline2d<tv> line1()const
	{
		return lline2d<tv>(p1, p2);
	}

	lline2d<tv> line2()const
	{
		return lline2d<tv>(p2, p3);
	}

	lline2d<tv> line3()const
	{
		return lline2d<tv>(p3, p1);
	}

	bool inrect(tv _x, tv _y)const
	{
		return rect().detect_point(_x, _y);
	}

	lrect<tv> rect()const
	{
		return lrect<tv>(
			lmin(p1.x, lmin(p2.x, p3.x)),
			lmin(p1.y, lmin(p2.y, p3.y)),
			lmax(p1.x, lmax(p2.x, p3.x)),
			lmax(p1.y, lmax(p2.y, p3.y))
		);
	}

	
};

// 2D���������
template <typename tv>
class lrect
{
public:
	union
	{
		struct
		{
			tv xmin, ymin, xmax, ymax;
		};

		struct
		{
			lvector2<tv> pmin, pmax;
		};
	};


	lrect() :xmin((tv)0), ymin((tv)0), xmax((tv)0), ymax((tv)0)
	{
	}

	lrect(const tv& _xmin, const tv& _ymin, const tv& _xmax, const tv& _ymax)
		:xmin(_xmin), ymin(_ymin), xmax(_xmax), ymax(_ymax)
	{
	}

	lrect(const lvector2<tv>& _pmin, const lvector2<tv>& _pmax)
		:pmin(_pmin), pmax(_pmax)
	{
	}

	lrect(const lrect& v)
		: pmin(v.pmin), pmax(v.pmax)
	{

	}

	lrect& operator = (const lrect& v)
	{
		pmin = v.pmin;
		pmax = v.pmax;
		return *this;
	}

	tv xlen()const
	{
		return xmax - xmin;
	}

	tv ylen()const
	{
		return ymax - ymin;
	}

	tv area()const
	{
		return xlen() * ylen();
	}

	// ��ȡ���� 
	lvector2<tv> point1()const
	{
		return pmin;
	}

	lvector2<tv> point2()const
	{
		return lvector2(xmin, ymax);
	}

	lvector2<tv> point3()const
	{
		return lvector2(xmax, ymax);
	}

	lvector2<tv> point4()const
	{
		return pmax;
	}

	// ���ӵ㣬���ž��η�Χ
	void add_point();

	bool detect_point(tv _x, tv _y)const
	{
		return inside(_x, xmin, xmax) && inside(_y, ymin, ymax);
	}

	// ������ε���ײ���
	// 0:�ھ����� 1 �ھ����� 2��λ�ھ��εı�
	// x ��xmin xmax֮�� y
	bool detect_point(const lvector2<tv>& v)
	{
		return inside(v.x, xmin, xmax,) && inside(v.y, ymin, ymax);
	}

	// ����һ�����ε���ײ���
	int detect_rect(const lrect<tv>& rec)
	{
		// x ��y ����֮�䣬������ײ ������һ������
		return ( 
			inside(xmin, rec.xmin, rec.xmax) || inside(xmax, rec.xmin, rec.xmax)
			) &&
			(
			inside(ymin, rec.ymin, rec.ymax) || inside(ymax, rec.ymin, rec.ymax)
			);
	}

	// �������ε���ײ���
	int detect_tri(const ltri2d<tv>& tri)
	{
		// ����������һ������
		return detect(tri.p1) || detect(tri.p2) || detect(tri.p3);
	}

	// �����ߵ���ײ���
	int detect_ray(const lvector2<tv>& vec)
	{
		// ������������ε���ײ���
		// ������һ�����ཻ
	}

	// �����ߵ���ײ���

	// ��Բ�ε���ײ���

	// 
	static lrect add(const lrect& r1, const lrect& r2)
	{
		return lrect(lvector2<tv>::make_min(r1.pmin, r2.pmin),
			lvector2<tv>::make_max(r1.pmax, r2.pmax));
	}

	static lrect make(const lvector2<tv>& p1, const lvector2<tv>& p2)
	{
		return lrect(lmin(p1.x, p2.x), lmin(p1.y, p2.y),
			lmax(p1.x, p2.x), lmax(p1.y, p2.y));
	}
};

// 2d�������ı���
template <typename tv>
class lquad
{
public:

};

// 2d������
template <typename tv>
class lray2d
{
public:
	// �ɵ�ͷ���ȷ��
	lvector2 point, direction;
};





// 2d��Բ��
template <typename tv>
class lcircle2d
{
public:
	lcircle2d() {}

	lcircle2d(const lvector2<tv>& _c , tv _r)
		:center(_c), radius(_r)
	{ }

	lvector2<tv> center;
	tv radius;

};