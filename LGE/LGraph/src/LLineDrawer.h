#pragma once

#include <math.h>
#include "LPointDrawer.h"

//////////////////////////////////////////////////////////////////////////
// 关于优化的几点实践：
// 1、在本机上，float的运算速度并不比int低
// 2：Bresenham效率反而底下
// 3、roundf ceil floor 有了这些，比直接强制转换，效率差20倍
// roundf 用if来判断？

// 只能用于正数
inline int myround(float f)
{
	return (f - (int)f > 0.5f) ? (int)f + 1 : (int)f;
}

// BS空间的线
class BSLineDrawer
{
public:
	// 起点 终点
	BSPointDrawer p1, p2;
	// 状态
	dword state;

	BSLineDrawer() = default;

	BSLineDrawer(const BSPointDrawer& _p1, const BSPointDrawer& _p2)
		: p1(_p1), p2(_p2), state(0)
	{
	}

	BSLineDrawer(const lline2di& line, const largb& cl)
	{
	}

	// 禁止复制构造 使用移动构造
	BSLineDrawer(const BSLineDrawer&) = delete;

	BSLineDrawer( BSLineDrawer&& v)
		:p1(v.p1), p2(v.p2), state(v.state), points(std::move(v.points))
	{ }

	BSLineDrawer& operator = (const BSLineDrawer&) = delete;

	BSLineDrawer& operator = ( BSLineDrawer&& v)
	{
		if (this != &v)
		{
			p1 = v.p1; p2 = v.p2; state = v.state;
			points = std::move(v.points);
		}
		return *this;
	}

	void Set(const BSPointDrawer& _p1, const BSPointDrawer& _p2)
	{
		p1 = _p1;
		p2 = _p2;
	}


	void Set(const lline2di& line, const largb& cl)
	{
		p1.Set(line.p1, cl);
		p2.Set(line.p2, cl);
	}

	void Create(const lline2di& line, const largb& cl1, const largb& cl2);


	//  像素点
	std::vector<BSPointDrawer> points;

	// 创建直线的像素点 自动选择方式 dx > dy就是x主导的
	void CreatePoints()
	{
		// x主导
		int dx = p2.x - p1.x;
		int dy = p2.y - p1.y;
		if (abs(dx) > abs(dy))
		{
			CreatePointsX();
		}
		else
		{
			CreatePointY();
		}
	}

	// 画线的颜色递推的方式
	void CreatePointsEX()
	{
		int dx = p2.x - p1.x;
		int dy = p2.y - p1.y;

		// x主导
		if (abs(dx) > abs(dy))
		{
			CreatePointsXEX();
		}
		else
		{
			CreatePointYEX();
		}
	}

	void CreatePointsX()
	{
		// 【计算像素点位置】
		// 横线的情况
		if (p1.y == p2.y)
		{
			points.resize(abs(p1.x - p2.x));
			int dx = p2.x > p1.x ? 1 : -1;
			int np = 0;
			for (int ix = p1.x; ix != p2.x; ix += dx)
			{
				points[np].x = ix;
				points[np].y = p1.y;
				np++;
			}
		}
		else
		{
			float x21f = (float)(p2.x - p1.x);
			int dx = p2.x > p1.x ? 1 : -1;
			float k = (float)(p2.y - p1.y) / x21f * dx;
			int np = 0;
			points.resize(abs(p2.x - p1.x));
			float iyf = (float)p1.y;

			for (int ix = p1.x; ix != p2.x; ix += dx)
			{
				points[np].y = (int)iyf;
				points[np].x = ix;
				iyf += k; np++;
			}
		}
		
		// 【计算颜色】
		if (state & ERenderState::eColor)
		{
			this->CreateColorsX();
		}

		return;
	}

	// 计算颜色
	void CreateColorsX()
	{
		const largb& cl1 = p1.cl;
		const largb& cl2 = p2.cl;
		// 颜色相同的情况
		if (cl1 == cl2)
		{
			for (auto& p : points)
				p.cl = cl1;
		}
		else
		{
			float x21f = (float)(p2.x - p1.x);
			int dx = p2.x > p1.x ? 1 : -1;
			float3 krgba(
				(float)(cl2.r - cl1.r) / x21f * dx, 
				(float)(cl2.g - cl1.g) / x21f * dx, 
				(float)(cl2.b - cl1.b) / x21f * dx
				//(float)(cl2.a - cl1.a) / x21f * dx 
			);
			float3 irgba(cl1.r, cl1.g, cl1.b);

			for (auto& p : points)
			{
				p.cl.r = (uchar)irgba.x;
				p.cl.g = (uchar)irgba.y;
				p.cl.b = (uchar)irgba.z;
				//p.cl.a = (uchar)irgba.w;
				irgba += krgba;
			}
		}
	}

	void CreatePointY()
	{
		if (p1.x == p2.x)
		{
			points.resize(abs(p2.y - p1.y));
			int np = 0;
			int dy = (p2.y > p1.y) ? 1 : -1;
			for (int iy = p1.y; iy != p2.y; iy+=dy)
			{
				points[np].x = p1.x;
				points[np].y = iy;
				np++;
			}
		}
		else
		{
			float y21f = (float)(p2.y - p1.y);
			float k = (float)(p2.x - p1.x) / y21f;
			int dy = p2.y > p1.y ? 1 : -1;
			k *= dy;
			int np = 0;
			points.resize(abs(p2.y - p1.y));
			float ixf = (float)p1.x;
			for (int iy = p1.y; iy != p2.y; iy += dy)
			{
				points[np].y = iy;
				points[np].x = (int)(ixf);
				ixf += k; np++;
			}
		}

		// 如果需要插值颜色 两种方法 两次循环或者一次循环完成
		if (state & ERenderState::eColor)
		{
			this->CreateColorsY();
		}
	}

	void CreateColorsY()
	{
		// 计算颜色的变化值
		// 浮点数的加法 四个分量分别
		const largb& cl1 = p1.cl;
		const largb& cl2 = p2.cl;

		if (cl1 == cl2)
		{
			for (auto & p:points)
			{
				p.cl = cl1;
			}
		}
		else
		{
			float y21f = (float)(p2.y - p1.y);
			int dy = p2.y > p1.y ? 1 : -1;
			float3 krgba(
				(float)(cl2.r - cl1.r) / y21f*dy, //float kr = ;
				(float)(cl2.g - cl1.g) / y21f*dy, //float kg = ;
				(float)(cl2.b - cl1.b) / y21f*dy //float kb = ;
				//(float)(cl2.a - cl1.a) / y21f*dy //float ka = ; 
			);

			float3 irgba(cl1.r, cl1.g, cl1.b);
			for (auto& p : points)
			{
				p.cl.r = (uchar)irgba.x;
				p.cl.g = (uchar)irgba.y;
				p.cl.b = (uchar)irgba.z;
				//p.cl.a = (uchar)irgba.w;
				irgba += krgba;
			}
		}
	}

	void CreatePointsXEX()
	{
		if (p1.y == p2.y)
		{
			int xmin = min(p1.x, p2.x);
			int xmax = max(p1.x, p2.x);
			points.resize(abs(xmax - xmin));
			int np = 0;
			for (int ix = xmin; ix < xmax; ix++)
			{
				points[np].x = ix;
				points[np].y = p1.y;
				np++;
			}
		}
		else
		{
			TLerper<float> ler(p1.x, p2.x, (float)p1.y, (float)p2.y);
			points.resize(ler.count());
			int np = 0;
			while (!ler.end())
			{
				points[np].x = ler.ai;
				points[np].y = (int)ler.bi;
				ler.next();
				np++;
			}
		}

		// 深度插值 深度测试的办法：不通过的点不加入表？但是颜色插值还是需要的
		// 【递推模式下，深度测试起不到优化效果。】
		// 每一个点都需要计算所有信息
		// 在非递推模式下，第一次循环，计算所有的xyz，如果z> depthbuf.getdepth(x,w)
		// 即测试不通过，点不加入表中 这个在三角形的扫描线中才启用
		// 三条边的，还是需要计算所有的
		if (state & ERenderState::eDepthWrite || state & ERenderState::eDepthTest)
		{
			TLerper<float> ler(p1.x, p2.x, p1.z, p2.z);
			int np = 0;
			while (!ler.end())
			{
				points[np].z = ler.bi;
				ler.next();
				np++;
			}
		}

		// 颜色插值 在光照开启的情况下，像素级光照和顶点光照两种选择方式
		// 像素光照，需要插值法线 float3的计算 然后每个像素需要计算光照颜色
		// 顶点级光照，顶点计算出光照颜色 插值顶点颜色
		if (state & ERenderState::eColor)
		{
			TLerper<float3> ler(p1.x, p2.x,
				float3(p1.cl.r, p1.cl.g, p1.cl.b),
				float3(p2.cl.r, p2.cl.g, p2.cl.b));
			int np = 0;
			largb cl;
			while (!ler.end())
			{
				cl.r = (uchar)ler.bi.x;
				cl.g = (uchar)ler.bi.y;
				cl.b = (uchar)ler.bi.z;
				points[np].cl = cl;
				ler.next();
				np++;
			}
		}

		// uv坐标插值
		if (state & ERenderState::eTex)
		{
			TLerper<float2> ler(p1.x, p2.x, p1.uv, p2.uv);
			int np = 0;
			while (!ler.end())
			{
				points[np].uv = ler.bi;
				ler.next();
				np++;
			}
		}
	}

	

	void CreatePointYEX()
	{
		if (p1.x == p2.x)
		{
			int ymin = min(p1.y, p2.y);
			int ymax = max(p1.y, p2.y);
			points.resize(ymax - ymin);
			int np = 0;
			for (int iy = ymin; iy < ymax; iy++)
			{
				points[np].x = p1.x;
				points[np].y = iy;
				np++;
			}
		}
		else
		{
			TLerper<float> ler(p1.y, p2.y, (float)p1.x, (float)p2.x);
			points.resize(ler.count());
			int np = 0;
			while (!ler.end())
			{
				points[np].x = (int)ler.bi;
				points[np].y = ler.ai;
				ler.next();
				np++;
			}
		}

		

		if (state & ERenderState::eColor)
		{
			TLerper<float3> ler(p1.y, p2.y,
				float3(p1.cl.r, p1.cl.g, p1.cl.b),
				float3(p2.cl.r, p2.cl.g, p2.cl.b));
			int np = 0;
			largb cl;
			while (!ler.end())
			{
				cl.r = (uchar)ler.bi.x;
				cl.g = (uchar)ler.bi.y;
				cl.b = (uchar)ler.bi.z;
				points[np].cl = cl;
				ler.next();
				np++;
			}
		}
		
	}


	//  计算各像素点的颜色每像素乘法 用时是CreateColorsXY加法递推的200倍
	void CreateColorsEX()
	{
		if (p1.cl == p2.cl)
		{
			for (auto& p : points)
			{
				p.cl = p1.cl;
			}
		}
		// 插值获取 对于点p 计算距离 w = dist(p, p1) / dis(p1, p2)
		// cl = (1- w) * cl1 + (w*cl2 即color.clamp(cl1, cl2, 1-w)
		if (this->BegPoint())
		{
			float2 fp1((float)p1.x, (float)p1.y);
			float2 fp2((float)p2.x, (float)p2.y);
			float dst12 = fp1.distance(fp2);
			for (auto& p : points)
			{
				float2 fp((float)p.x, (float)p.y);
				float dstp1 = fp.distance(fp1);
				float w = dstp1 / dst12;
				p.cl = largb::lerp(p2.cl, p1.cl, w);
			}

		}
	}

	// 贴图

	// 遍历像素点
	bool BegPoint()
	{
		itP = points.begin();
		return itP != points.end();
	}

	BSPointDrawer* NextPoint()
	{
		if ( itP != points.end())
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

	// 点的数量
	size_t PointCount()const
	{
		return points.size();
	}

	// 绘制
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

	// 弃用 不知为何，效率反而低一半
	void _CreatePointsBre()
	{
		// 横线的情况 这种情况教多，扫描线
		if (p1.y == p2.y)
		{
			int xmin = min(p1.x, p2.x);
			int xmax = max(p1.x, p2.x);
			points.resize(abs(xmax - xmin));
			int np = 0;
			for (int ix = xmin; ix < xmax; ix++)
			{
				points[np].x = ix;
				points[np].y = p1.y;
			}
			return;
		}

		// 竖线的情况
		if (p1.x == p2.x)
		{
			int ymin = min(p1.y, p2.y);
			int ymax = max(p1.y, p2.y);
			points.resize(ymax - ymin);
			int np = 0;
			for (int iy = ymin; iy < ymax; iy++)
			{
				points[np].x = p1.x;
				points[np].y = iy;
			}
			return;
		}

		//
		int dx = p2.x - p1.x;
		int dy = p2.y - p1.y;

		if (abs(dx) > abs(dy))
		{
			LLineBreX bre(p1.x, p1.y, p2.x, p2.y);
			int ix = p1.x; int iy = p1.y;
			points.resize(bre.dx);
			for (int i = 0; i < bre.dx; i++)
			{
				bre.P(ix, iy);
				points[i].x = ix;
				points[i].y = iy;
			}
		}
		else
		{
			LLineBreY bre(p1.x, p1.y, p2.x, p2.y);
			int ix = p1.x; int iy = p1.y;
			points.resize(bre.dy);
			for (int i = 0; i < bre.dy; i++)
			{
				bre.P(ix, iy);
				points[i].x = ix;
				points[i].y = iy;
			}
		}

		return;
	}

};

