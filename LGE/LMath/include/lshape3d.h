#pragma once

#include "lvector3.h"
#include "lshape2d.h"
#include <vector>

//////////////////////////////////////////////////////////////////////////
// ��ǰ����
// ����
// 3d�����򳤷���
template <typename tv>
class labox;
// 3d�����ⳤ����
template <typename tv>
class lbox;
template <typename tv>
class lsphere;
// 3d������
template <typename tv>
class lray3d;
// 3d���߶�
template <typename tv>
class lline3d;
// 3d��������
template <typename tv>
class ltri3d;
	// ƽ��
template <typename tv>
class lplane;
	// ��׶��
template <typename tv>
class lfrustum;

// 3d�����򳤷���
template <typename tv>
class labox
{
public:

};

// ���ֱ�߷���

// 3d�����ⳤ����
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

	// ���ֱ����box�Ľ��� �������?
	int intersection(const lline3d<tv>& line)
	{

	}

	// �ü�ֱ�� �����Ƿ�����
	bool cull(const lline3d<tv>& line, lline3d<tv> line_out)
	{
		// ֻ����xyƽ��

	}

	// ����box�Ľ���
	tv3 intersection(const tv3& p1, const tv3& p2);

	// ����ƽ����вü�

	// �ü������
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
				// ���㶼���� ���p1
				outp.push_back(p1);
			}
			else if (bp1 && (!bp2))
			{
				// p1�ڣ� p2���� ���p1 �� xmin�Ľ���
				outp.push_back(p1);
				outp.push_back(tl(p1, p2).px(pmin.x));
			}
			else if ((!bp1) && bp2)
			{
				// p1���� p2���� �������
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
				// ���㶼���� ���p1
				outp.push_back(p1);
			}
			else if (bp1 && (!bp2))
			{
				// p1�ڣ� p2���� ���p1 �� xmax�Ľ���
				outp.push_back(p1);
				outp.push_back(tl(p1, p2).px(pmax.x));
			}
			else if ((!bp1) && bp2)
			{
				// p1���� p2���� �������
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
				// ���㶼���� ���p1
				outp.push_back(p1);
			}
			else if (bp1 && (!bp2))
			{
				// p1�ڣ� p2���� ���p1 �� ymin�Ľ���
				outp.push_back(p1);
				outp.push_back(tl(p1, p2).py(pmin.y));
			}
			else if ((!bp1) && bp2)
			{
				// p1���� p2���� �������
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
				// ���㶼���� ���p1
				outp.push_back(p1);
			}
			else if (bp1 && (!bp2))
			{
				// p1�ڣ� p2���� ���p1 �� ymax�Ľ���
				outp.push_back(p1);
				outp.push_back(tl(p1, p2).py(pmax.y));

			}
			else if ((!bp1) && bp2)
			{
				// p1���� p2���� �������
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
				// ���㶼���� ���p1
				outp.push_back(p1);
			}
			else if (bp1 && (!bp2))
			{
				// p1�ڣ� p2���� ���p1 �� zmin�Ľ���
				outp.push_back(p1);
				outp.push_back(tl(p1, p2).pz(pmin.z));
			}
			else if ((!bp1) && bp2)
			{
				// p1���� p2���� �������
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
				// ���㶼���� ���p1
				outp.push_back(p1);
			}
			else if (bp1 && (!bp2))
			{
				// p1�ڣ� p2���� ���p1 �� zmax�Ľ���
				outp.push_back(p1);
				outp.push_back(tl(p1, p2).pz(pmax.z));
			}
			else if ((!bp1) && bp2)
			{
				// p1���� p2���� �������
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

	// �ü������
	std::vector<tv3> cull(const std::vector<tv3>& in_p)const
	{
		return cullx(cully(cullz(in_p)));
	}

	// �ü�������
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

// ����
template <typename tv>
class lsphere
{

};



// 3d������
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

	// ����2D��
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

	// ��֪x ����ֱ���ϵĵ�
	tv3 px(tv _x)
	{
		auto w = lweight(p1.x, p2.x, _x);
		return tv3(_x,
			llerp(p1.y, p2.y, w),
			llerp(p1.z, p2.z, w));
	}

	// ��֪y ����ֱ���ϵĵ�
	tv3 py(tv _y)
	{
		auto w = lweight(p1.y, p2.y, _y);
		return tv3(llerp(p1.x, p2.x, w),
			_y,
			llerp(p1.z, p2.z, w));
	}

	// ��֪z ����ֱ���ϵĵ�
	tv3 pz(tv _z)
	{
		auto w = lweight(p1.z, p2.z, _z);
		return tv3( llerp(p1.x, p2.x, w),
			llerp(p1.y, p2.y, w),
			_z);
	}
};


// 3d��������
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


// ƽ��
template <typename tv>
struct lplane
{
	typedef lvector3<tv> t_point;
	typedef lline3d<tv> t_line;
	t_point p1, p2, p3;

	// �����㹹��һ��ƽ��

	// ������

	// ֱ����Ϊ���ߣ��ͷ����ϵ�һ���㣬����һ��ƽ��
};


// ��׶��
template <typename tv>
class lfrustum
{
public:
	// �������ε���ײ���
	bool detect(const ltri3d<tv>& tri);
};