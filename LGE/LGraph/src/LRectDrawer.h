#pragma once

#include "LLineDrawer.h"
#include "lmath.hpp"

// 矩形绘制

class BSRectDrawer
{
public:
	lrecti rect;
	// 分别为 (xmin, ymin) (xmax, ymin) (xmax, ymax) (xmin, ymax)
	largb cl1, cl2, cl3, cl4;
	std::vector<BSPointDrawer> points;

	BSRectDrawer(const lrecti& _rec, largb _cl1, largb _cl2, largb _cl3, largb _cl4)
		:rect(_rec), cl1(_cl1), cl2(_cl2), cl3(_cl3), cl4(_cl4)
	{

	}

	void CreatePoints()
	{
		points.resize(rect.area());
		int nl = 0;
		for (int iy = rect.ymin; iy < rect.ymax; iy++)
		{
			for (int ix = rect.xmin; ix <rect.xmax; ix++)
			{
				points[nl].x = ix;
				points[nl].y = iy;
				// 颜色插值:
				float wx = (float)(rect.xmax - ix) / (float)rect.xlen();
				float wy = (float)(rect.ymax - iy) / (float)rect.ylen();
				largb cldown = largb::lerp(cl1, cl2, wx);
				largb clup = largb::lerp(cl4, cl3, wx);
				points[nl].cl = largb::lerp(cldown, clup, wy);
				nl++;
			}
		}
		assert(nl == points.size());
	}

	// 遍历像素点
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


	// 绘制
	void DrawTo(LColorBuffer* ptarget, LDepthBuffer* pdethbuf)
	{
		// 绘制填充内容
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
	std::vector<BSPointDrawer>::iterator itP;
};

class BSRectDrawerL
{
public:
	lrecti rect;
	// 分别为 (xmin, ymin) (xmax, ymin) (xmax, ymax) (xmin, ymax)
	largb cl;
	std::vector<BSPointDrawer> points;

	BSRectDrawerL(const lrecti& _rec, largb _cl)
		: rect(_rec), cl(_cl)
	{ }

	void CreatePoints()
	{
		points.resize(rect.xlen()*2 +rect.ylen()*2);
		int nl = 0;
		for (int ix = rect.xmin; ix < rect.xmax; ix++)
		{
			points[nl].x = ix;
			points[nl].y = rect.ymin;
			points[nl].cl = cl;
			nl++;
			points[nl].x = ix;
			points[nl].y = rect.ymax;
			points[nl].cl = cl;
			nl++;
		}

		for (int iy = rect.ymin; iy < rect.ymax; iy++)
		{
			points[nl].x = rect.xmin;
			points[nl].y = iy;
			points[nl].cl = cl;
			nl++;
			points[nl].x = rect.xmax;
			points[nl].y = iy;
			points[nl].cl = cl;
			nl++;
		}
		assert(nl == points.size());
	}

	// 遍历像素点
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


	// 绘制
	void DrawTo(LColorBuffer* ptarget, LDepthBuffer* pdethbuf)
	{
		// 绘制填充内容
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
	std::vector<BSPointDrawer>::iterator itP;
};