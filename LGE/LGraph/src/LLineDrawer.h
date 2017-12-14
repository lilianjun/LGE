#pragma once

#include <math.h>
#include "LPointDrawer.h"

//////////////////////////////////////////////////////////////////////////
// �����Ż��ļ���ʵ����
// 1���ڱ����ϣ�float�������ٶȲ�����int��
// 2��BresenhamЧ�ʷ�������
// 3��roundf ceil floor ������Щ����ֱ��ǿ��ת����Ч�ʲ�20��
// roundf ��if���жϣ�

// ֻ����������
inline int myround(float f)
{
	return (f - (int)f > 0.5f) ? (int)f + 1 : (int)f;
}

// BS�ռ����
class BSLineDrawer
{
public:
	// ��� �յ�
	BSPointDrawer p1, p2;
	// ״̬
	dword state;

	BSLineDrawer() = default;

	BSLineDrawer(const BSPointDrawer& _p1, const BSPointDrawer& _p2)
		: p1(_p1), p2(_p2), state(0)
	{
	}

	BSLineDrawer(const lline2di& line, const largb& cl)
	{
	}

	// ��ֹ���ƹ��� ʹ���ƶ�����
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


	//  ���ص�
	std::vector<BSPointDrawer> points;

	// ����ֱ�ߵ����ص� �Զ�ѡ��ʽ dx > dy����x������
	void CreatePoints()
	{
		// x����
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

	// ���ߵ���ɫ���Ƶķ�ʽ
	void CreatePointsEX()
	{
		int dx = p2.x - p1.x;
		int dy = p2.y - p1.y;

		// x����
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
		// ���������ص�λ�á�
		// ���ߵ����
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
		
		// ��������ɫ��
		if (state & ERenderState::eColor)
		{
			this->CreateColorsX();
		}

		return;
	}

	// ������ɫ
	void CreateColorsX()
	{
		const largb& cl1 = p1.cl;
		const largb& cl2 = p2.cl;
		// ��ɫ��ͬ�����
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

		// �����Ҫ��ֵ��ɫ ���ַ��� ����ѭ������һ��ѭ�����
		if (state & ERenderState::eColor)
		{
			this->CreateColorsY();
		}
	}

	void CreateColorsY()
	{
		// ������ɫ�ı仯ֵ
		// �������ļӷ� �ĸ������ֱ�
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

		// ��Ȳ�ֵ ��Ȳ��Եİ취����ͨ���ĵ㲻�����������ɫ��ֵ������Ҫ��
		// ������ģʽ�£���Ȳ����𲻵��Ż�Ч������
		// ÿһ���㶼��Ҫ����������Ϣ
		// �ڷǵ���ģʽ�£���һ��ѭ�����������е�xyz�����z> depthbuf.getdepth(x,w)
		// �����Բ�ͨ�����㲻������� ����������ε�ɨ�����в�����
		// �����ߵģ�������Ҫ�������е�
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

		// ��ɫ��ֵ �ڹ��տ���������£����ؼ����պͶ����������ѡ��ʽ
		// ���ع��գ���Ҫ��ֵ���� float3�ļ��� Ȼ��ÿ��������Ҫ���������ɫ
		// ���㼶���գ���������������ɫ ��ֵ������ɫ
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

		// uv�����ֵ
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


	//  ��������ص����ɫÿ���س˷� ��ʱ��CreateColorsXY�ӷ����Ƶ�200��
	void CreateColorsEX()
	{
		if (p1.cl == p2.cl)
		{
			for (auto& p : points)
			{
				p.cl = p1.cl;
			}
		}
		// ��ֵ��ȡ ���ڵ�p ������� w = dist(p, p1) / dis(p1, p2)
		// cl = (1- w) * cl1 + (w*cl2 ��color.clamp(cl1, cl2, 1-w)
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

	// ��ͼ

	// �������ص�
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

	// �������
	size_t PointCount()const
	{
		return points.size();
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

	// ���� ��֪Ϊ�Σ�Ч�ʷ�����һ��
	void _CreatePointsBre()
	{
		// ���ߵ���� ��������̶࣬ɨ����
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

		// ���ߵ����
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

