#pragma once

// Բ�εĻ���

#include "lmath.hpp"
#include "LSurface.h"
#include "LPointDrawer.h"

class BSCircleDrawer
{
public:
	lcircle2di circle;
	// ���ĵ���Ե����ɫ
	largb cl1, cl2;
	BSCircleDrawer()
	{ }

	std::vector<BSLineDrawer> scanLines;

	BSCircleDrawer(const lcircle2di& _circle, largb _cl1, largb _cl2)
		: circle(_circle), cl1(_cl1), cl2(_cl2)
	{ }

	void Set(const lcircle2di& _circle, largb _cl1, largb _cl2)
	{
		circle = _circle;
		cl1 = _cl1;
		cl2 = _cl2;
	}


	// �������ĵ�
	void CreatePoints()
	{
		// x y �����Կ�
		const int& r = circle.radius;
		const int& x0 = circle.center.x;
		const int& y0 = circle.center.y;
		int ix = 0;
		float f;
		int y1, y2;
		scanLines.resize(r * 2 + 1);
		int nl = 0;
		for (int i = -r; i < r + 1 ; i++)
		{
			ix = circle.center.x + i;
			//yi = +-sqrt(r*r - (xi - x0) ^ 2) + y0
			f = sqrtf(r * r - (ix - x0) * (ix - x0));
			y1 = (int)roundf(y0 + f);
			y2 = (int)roundf(y0 - f);
			scanLines[nl].p1.x = ix;
			scanLines[nl].p1.y = y1;
			scanLines[nl].p2.x = ix;
			scanLines[nl].p2.y = y2;
			scanLines[nl].CreatePoints();
			nl++;
		}

		CreateColors();
	}

	void CreateColors()
	{
		// ��ɫ��ȵ����
		if (cl1 == cl2)
		{
			if (BegPoint())
			{
				auto p = NextPoint();
				while (p)
				{
					p->cl = cl1;
					p = NextPoint();
				}
			}
		}
		else
		{
			if (BegPoint())
			{
				auto p = NextPoint();
				while (p)
				{
					// ����� ���԰뾶
					float w = sqrtf((p->x - circle.center.x) * (p->x - circle.center.x)
						+ (float)(p->y - circle.center.y) * (p->y - circle.center.y)) / (float)circle.radius;
					p->cl = largb::lerp(cl1, cl2, (1.f - lclamp(w, 0.f, 1.f)));
					p = NextPoint();
				}
			}
		}
		
	}

	// �������ص�
	bool BegPoint()
	{
		itScanLine = scanLines.begin();

		return itScanLine != scanLines.end();
	}

	BSPointDrawer* NextPoint()
	{
		BSPointDrawer* p = itScanLine->NextPoint();
		if (p)
		{
			return p;
		}
		else
		{
			itScanLine++;
			// ���һֱ�ǿ��� �ҵ�
			while (itScanLine != scanLines.end() && (!itScanLine->BegPoint()))
			{
				itScanLine++;
			}

			if (itScanLine == scanLines.end())
			{
				return nullptr;
			}
			else
			{
				return itScanLine->NextPoint();
			}

		}
		return nullptr;
	}


	// ����
	void DrawTo(LColorBuffer* ptarget, LDepthBuffer* pdethbuf)
	{
		// �����������
		if (BegPoint())
		{
			BSPointDrawer* p = NextPoint();
			while (p)
			{
				p->DrawTo(ptarget, pdethbuf);
				p = NextPoint();
			}
		}
	}

private:
	std::vector<BSLineDrawer>::iterator itScanLine;
};

// û����� ֻ�����ߵ�Բ
class BSCircleLineDrawer
{
public:
	lcircle2di circle;
	
	std::vector<BSPointDrawer> points;
	
	largb cl;
	
	BSCircleLineDrawer()
	{
	}


	BSCircleLineDrawer(const lcircle2di& _circle, largb _cl)
		: circle(_circle), cl(_cl)
	{
	}

	void Set(const lcircle2di& _circle, largb _cl)
	{
		circle = _circle;
		cl = _cl;
	}

	void CreatePoints()
	{
		int rr = circle.radius * circle.radius;
		int range = (int)roundf((int)(sqrt(2.f) * (float)circle.radius) / 2.0f);
		points.resize((range * 2 + 1) * 4);
		int np = 0;
		//
		for (int i = -range; i < range + 1; i++)
		{
			// X������ x (-r*sqrt(2)�� r*sqrt(2))
			int ix1 = circle.center.x + i;
			// yi = + - sqrt(r*r - (xi - x0) ^ 2 ) + y0
			int dv = (ix1 - circle.center.x);
			float f = sqrtf((float)rr - dv * dv);
			points[np].x = ix1;
			points[np].y = (int)roundf(circle.center.y + f);
			points[np].cl = cl;
			np++;
			points[np].x = ix1;
			points[np].y = (int)roundf(circle.center.y - f);
			points[np].cl = cl;
			np++;

			// Y������ y (-r*sqrt(2)�� r*sqrt(2))
			// xi = + 1 sqrt(r*r - (yi - y0) ^ 2) + x0
			int iy1 = circle.center.y + i;
			dv = iy1 - circle.center.y;
			f = sqrtf((float)rr - dv * dv);
			points[np].x = (int)roundf(circle.center.x + f);
			points[np].y = iy1;
			points[np].cl = cl;
			np++;
			points[np].x = (int)roundf(circle.center.x - f);
			points[np].y = iy1;
			points[np].cl = cl;
			np++;

		}


	}

	// �������ص�
	bool BegPoint()
	{
		itP = points.begin();
		return itP != points.end();
	}

	BSPointDrawer* NextPoint()
	{
		if (itP != points.end())
		{
			auto p = itP._Ptr;
			itP++;
			return p;
		}
		else
		{
			return nullptr;
		}
	}

	// ����
	void DrawTo(LColorBuffer* ptarget, LDepthBuffer* pdethbuf)
	{
		if (BegPoint())
		{
			auto p = NextPoint();
			while (p)
			{
				p->DrawTo(ptarget, pdethbuf);
				p = NextPoint();
			}
		}
	}

private:
	std::vector<BSPointDrawer>::iterator itP;
};