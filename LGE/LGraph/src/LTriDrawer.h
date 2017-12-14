#pragma once

#include "lmath.hpp"

#include "LSurface.h"
#include "LLineDrawer.h"

// ������ˮ��
template <typename t_ver_list_prc, typename t_prim_list_prc, typename  t_pro_list_prc>
struct lvertex_pipeline
{
	t_ver_list_prc ver_list_prc;
	t_prim_list_prc prim_list_prc;
	t_pro_list_prc pro_list_prc;

	bool operator()()
	{
		assert(0);
		return false;
	}
};

// ������ˮ��
template <typename t_prim_list_drawer>
class lpixel_pipeline
{
	t_prim_list_drawer prim_list_drawer;
	bool operator()();
};


// �����ϣ�һ�������͵������������ϳ�����Ⱦ������С������κβ��ֶ������滻
// ȫ�����Ƿ��͵ġ�

// ֻ��λ�� 
struct stVPos
{
	float3 pos;
};

// ����ɫֵ ��ɫֵ�����ڶ���diffuse 
struct stVPosCl
{
	float3 pos; largb cl;
};


// ������
struct stSampler
{
	largb operator()(const float2& uv)
	{ }
};

// ���ؽ׶ε� ��������
struct stPSInput
{
	float z; largb cl;

	// ��դ���У�һ����Ҫ�õ���ֵ����ֵ�õ�����ֵ

	// һ��Ҫ�õ���ɫ����Ҫ������ɫ
	stPSInput operator + (const stPSInput& v)const
	{
		return stPSInput{ z + v.z, cl + v.cl };
	}

	stPSInput operator * (float f)const
	{
		return stPSInput{ z * f, cl *f };
	}

	stPSInput operator - (const stPSInput& v)const
	{
		return stPSInput{ z - v.z, cl - v.cl };
	}
};

// ������ɫ��
// ���뷺�͵�stPSInput ���color
// �в�ͬ�Ľṹ����ͬ���㷨
struct stPixelShader
{
	template <typename TPro>
	largb operator()(const float2& p, const TPro& pro)
	{
		std::cout << "������ɫ����ʼ����������" << std::endl;
		return pro.cl;
	}
};

// �ṩһ�����͵������λ����� ��ֵ�����εĸ�����ֵ
// TProperty�Ƕ�������� ��Ҫ�ṩ��operator + *float ���ڲ�ֵ
// ������һ�����ص�lerp������
// �ڶ��㣬��Ҫ����operator(const float2&) �������ص��λ����Ϣ
template<typename TProperty>
struct LTriDrawer
{
	float2 p1, p2, p3;
	TProperty pro1, pro2, pro3;

	template <typename TShader>
	void Draw(TShader sd)
	{
		// �������� Y��С����
		if (p1.y > p2.y)
		{
			lswap(p1, p2);
		}
		if (p2.y > p3.y)
		{
			lswap(p2, p3);
		}
		if (p1.y > p2.y)
		{
			lswap(p1, p2);
		}

		TLerper<float> l12(p1.y, p2.y, p1.x, p2.x);
		TLerper<float> l13(p1.y, p3.y, p1.x, p3.x);

		TLerper<TProperty> pro12(p1.y, p2.y, pro1, pro2);
		TLerper<TProperty> pro13(p1.y, p3.y, pro1, pro3);

		TLerper<TProperty> pro23(p2.y, p3.y, pro2, pro3);
		TLerper<float> l23(p2.y, p3.y, p2.x, p3.x);

		// ��ת��ʼ������
		TLerper<float>* lineL, *lineR;
		TLerper<TProperty>* proL, *proR;
		TProperty ipro;
		float2 ipoint;
		while (true)
		{
			// �˳�����
			// �л�������
			float iy = lineL->ai;
			float ixL = lineL->bi;
			float ixR = lineR->bi;
			// 
			float ix = ixL;
			TLerper<TProperty> proLR(ixL, ixR, proL->bi, proR->bi);
			while (!lequal(ix, ixR))
			{
				ipoint = float2(ix, iy);
				ipro = proLR.bi;
				sd(ipoint, ipro);
				proLR.next();
			}
			lineL->next();
			lineR->next();
			proL->next();
			proR->next();
		}
	}
};

template<typename T>
LTriDrawer<T> lmake_tridrawer(const float2& p1, const float2& p2, const float2& p3,
	const T& pr1, const T& pr2, const T& pr3)
{
	return LTriDrawer<T>{p1, p2, p3, pr1, pr2, pr3};
}

// ���嶥����ɫ��

// ���ṩһ�����������൱��������ɫ���Ķ����������������
// ����operator()


// �����λ����� ���ƾ�ͶӰ�任���ӿڲü����������
struct LTriDrawerX
{
	stVPosCl ver1, ver2, ver3;

	// �ӿ�ת������
	inline lint2 ViewPort(const float2& v, int w, int h)
	{
		return lint2((int)(v.x * w + w / 2.f), (int)(v.y * h + h / 2.f));
	}



	void DrawTo(const dword* ptar, const float* pdepth, int w, int h);

	private:
		static 	inline float3 _tof3(const largb& cl)
		{
			return float3(cl.r, cl.g, cl.g);
		}

		static inline largb _fromf3(const float3& f3)
		{
			return largb((uchar)f3.x, (uchar)f3.y, (uchar)f3.z);
		}
};

// BS�ռ�������λ��ƶ���
class BSTriDrawer
{
public:
	// ������
	BSPointDrawer p1, p2, p3;

	// ������
	BSLineDrawer line12;
	BSLineDrawer line23;
	BSLineDrawer line13;

	// 2d������
	ltri2di triangle;

	// ���η�Χ
	lrecti rect;
	
	// ������������ķ�ĸ
	int denominator;

	// ��Ⱦ״̬
	dword state;

	BSTriDrawer()
	{}

	BSTriDrawer(const BSPointDrawer& _p1, const BSPointDrawer& _p2, const BSPointDrawer& _p3)
		:p1(_p1), p2(_p2), p3(_p3), 
		triangle(lint2(_p1.x, _p1.y), lint2(_p2.x, _p2.y), lint2(_p3.x, _p3.y))
	{
		rect = triangle.rect();
	}

	// ��ֹ���ƹ���
	BSTriDrawer(const BSTriDrawer&) = delete;

	// ��ֹ���Ƹ�ֵ
	BSTriDrawer& operator = (const BSTriDrawer&) = delete;

	// ת�ƹ���
	BSTriDrawer(const BSTriDrawer&& v )
	{ }

	// ת�Ƹ�ֵ
	BSTriDrawer& operator = (const BSTriDrawer&& v)
	{
		return *this;
	}

	// ɨ���� �����ε����ص�洢��ɨ������
	// ɨ������y�ӵ͵������е�
	std::vector<BSLineDrawer> scanLines;

	void SetVer(const float3& v1, const float3& v2, const float3& v3)
	{
		p1.x = (int)v1.x; p2.x = (int)v2.x; p3.x = (int)v3.x;
		p1.y = (int)v1.y; p2.y = (int)v2.y; p3.y = (int)v3.y;
		p1.z = v1.z; p2.z = v2.z; p3.z = v3.z;
	}

	void SetCl(const largb& cl1, const largb& cl2, const largb& cl3)
	{
		p1.cl = cl1; p2.cl = cl2; p3.cl = cl3;
	}

	void SetUV(const float2& uv1, const float2& uv2, const float2& uv3)
	{
		p1.uv = uv1; p2.uv = uv1; p3.uv = uv3;
	}

	void SetState(dword s)
	{
		state = s;
	}

	// ���������ε����ص�
	void CreatePoints()
	{
		// �������� Y��С����
		if (p1.y > p2.y)
		{
			lswap(p1, p2);
		}
		if (p2.y > p3.y)
		{
			lswap(p2, p3);
		}
		if (p1.y > p2.y)
		{
			lswap(p1, p2);
		}

		denominator = (p1.y - p3.y) * (p2.x - p3.x) + (p2.y - p3.y) * (p3.x - p1.x);

		// ��С���� ������ ���µ���
		// ���������p1�� p2�� p3�� 
		// p1�� p2�� p3 ��

		// ����������
		line13.Set(p1, p3);
		line12.Set(p1, p2);
		line23.Set(p2, p3);
		line12.state = state;
		line23.state = state;
		line13.state = state;
		line12.CreatePointY();
		line23.CreatePointY();
		line13.CreatePointY();

		// ɨ���ߵ�����Ϊ�߶� ҲΪ���ߵ���������
		scanLines.resize(line13.PointCount());

		// ��ʼ�°��ƽ��������
		line12.BegPoint();
		line13.BegPoint();
		// 12���ϵĵ�
		BSPointDrawer* p12 = line12.NextPoint();
		// 13���ϵĵ�
		BSPointDrawer* p13 = line13.NextPoint();
		int nline = 0;
		while (p12)
		{
			// ������
			if (p12->x < p13->x)
			{
				scanLines[nline].p1 = *p12;
				scanLines[nline].p2 = *p13;
			}
			else
			{
				scanLines[nline].p1 = *p13;
				scanLines[nline].p2 = *p12;
			}
			scanLines[nline].state = state;
			scanLines[nline].CreatePoints();
			p13 = line13.NextPoint();
			p12 = line12.NextPoint();
			nline++;
		}
		// ������Ӧ�õ���p2��ˮƽ��
		
		// ��ʼ�ϰ��ƽ��������
		line23.BegPoint();
		BSPointDrawer* p23 = line23.NextPoint();
		//p13 = line13.NextPoint();
		
		while (p13)
		{
			if (p23->x < p13->x)
			{
				scanLines[nline].p1 = *p23;
				scanLines[nline].p2 = *p13;
			}
			else
			{
				scanLines[nline].p1 = *p13;
				scanLines[nline].p2 = *p23;
			}
			
			scanLines[nline].state = state;
			scanLines[nline].CreatePoints();
			p13 = line13.NextPoint();
			p23 = line23.NextPoint();
			nline++;
		}

		// ������ɫ
		//CreateColors();
	}


	// �����εĵڶ��ִ�����ʽ ��ɫֵҲ�ǽ��е���
	void CreatePointsEX()
	{
		// �������� Y��С����
		if (p1.y > p2.y)
		{
			lswap(p1, p2);
		}
		if (p2.y > p3.y)
		{
			lswap(p2, p3);
		}
		if (p1.y > p2.y)
		{
			lswap(p1, p2);
		}

		line13.Set(p1, p3);
		line12.Set(p1, p2);
		line23.Set(p2, p3);
		line12.state = state;
		line23.state = state;
		line13.state = state;
		line12.CreatePointYEX();
		line23.CreatePointYEX();
		line13.CreatePointYEX();

		// ɨ���ߵ�����Ϊ�߶� ҲΪ���ߵ���������
		scanLines.resize(line13.PointCount());

		// ��ʼ�ϰ��ƽ��������
		line12.BegPoint();
		line13.BegPoint();
		// 12���ϵĵ�
		BSPointDrawer* p12 = line12.NextPoint();
		// 13���ϵĵ�
		BSPointDrawer* p13 = line13.NextPoint();
		int nline = 0;
		while (p12)
		{
			// ������
			if (p12->x < p13->x)
			{
				scanLines[nline].p1 = *p12;
				scanLines[nline].p2 = *p13;
			}
			else
			{
				scanLines[nline].p1 = *p13;
				scanLines[nline].p2 = *p12;
			}

			scanLines[nline].state = state;
			scanLines[nline].CreatePointsEX();
			p13 = line13.NextPoint();
			p12 = line12.NextPoint();
			nline++;
		}
		// ������Ӧ�õ���p2��ˮƽ��

		// ��ʼ�°��ƽ��������
		line23.BegPoint();
		BSPointDrawer* p23 = line23.NextPoint();
		//p13 = line13.NextPoint();

		while (p13)
		{
			if (p23->x < p13->x)
			{
				scanLines[nline].p1 = *p23;
				scanLines[nline].p2 = *p13;
			}
			else
			{
				scanLines[nline].p1 = *p13;
				scanLines[nline].p2 = *p23;
			}

			scanLines[nline].state = state;
			scanLines[nline].CreatePointsEX();
			p13 = line13.NextPoint();
			p23 = line23.NextPoint();
			nline++;
		}
	}

	inline float _getw1(int x, int y)
	{
		return (float)((y - p3.y) * (p2.x - p3.x) + (p2.y - p3.y) * (p3.x - x)) / (float)denominator;
	}

	inline float _getw2(int x, int y)
	{
		return	(float)((y - p1.y) * (p3.x - p1.x) + (p3.y - p1.y) * (p1.x - x)) / (float)denominator;
	}

	inline float _getw3(int x, int y)
	{
		return	(float)((y - p2.y) * (p1.x - p2.x) + (p1.y - p2.y) * (p2.x - x)) / (float)denominator;
	}

	// ������������
	float3 GetWeightCoord(int x, int y)
	{
		float w1 = (_getw1(x, y));
		float w2 = (_getw2(x, y));
		float w3 = (_getw3(x, y));
		return float3(w1, w2, w3).getclamp(0.f, 1.f);
	}

	// ���������θ����ص����ɫ
	void CreateColors()
	{
		if (p1.cl == p2.cl && p1.cl == p3.cl)
		{
			if (BegPoint())
			{
				auto p = NextPoint();
				while (p)
				{
					p->cl = p1.cl;
					p = NextPoint();
				}
			}
			return;
		}

		// ��β�ֵ��ȡ?
		// ����������ľ������Ȩ��
		// ��������
		if (this->BegPoint())
		{
			auto p = NextPoint();
			while (p)
			{
				float3 w = GetWeightCoord(p->x, p->y);
				p->cl = w.x * p1.cl + w.y * p2.cl + w.z * p3.cl;
				p = NextPoint();
			}
		}
	}

	// ��������ص��UV����
	// ���ַ���������һ���ڼ����ʱ�����ơ�������������������
	void CreateUV()
	{
		// 
		if (this->BegPoint())
		{
			auto p = NextPoint();
			while (p)
			{
				float3 w = GetWeightCoord(p->x, p->y);
				//p->cl = w.x * p1.cl + w.y * p2.cl + w.z * p3.cl;
				p->uv = p1.uv * w.x + p2.uv * w.x + p3.uv * w.z;
				p = NextPoint();
			}
		}
	}

	// �������ص�
	bool BegPoint()
	{
		itScanLine = scanLines.begin();
		if (itScanLine != scanLines.end())
		{
			itScanLine->BegPoint();
			return true;
		}
		return false;
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



	inline float3 tof3(const largb& cl)
	{
		return float3(cl.r, cl.g, cl.g);
	}

	inline largb fromf3(const float3& f3)
	{
		return largb((uchar)f3.x, (uchar)f3.y, (uchar)f3.z);
	}

	void DrawToEX(LColorBuffer* ptarget, LDepthBuffer* pdethbuf)
	{
		// �������� Y��С����
		if (p1.y > p2.y)
		{
			lswap(p1, p2);
		}
		if (p2.y > p3.y)
		{
			lswap(p2, p3);
		}
		if (p1.y > p2.y)
		{
			lswap(p1, p2);
		}

		TLerper<float> l12( p1.y, p2.y, p1.x, p2.x );
		TLerper<float> l13( p1.y, p3.y, p1.x, p3.x );

		TLerper<float3> cl12(p1.y, p2.y, tof3(p1.cl), tof3(p2.cl));
		TLerper<float3> cl13(p1.y, p3.y, tof3(p1.cl), tof3(p3.cl));

		int x1, x2, ix;
		TLerper<float3> cllr;
		for (int h = 0; h < l12.count(); h++)
		{
			x1 = (int)l12.bi;
			x2 = (int)l13.bi;
			// ������
			if (x1 < x2)
			{
				cllr.set(x1, x2, cl12.bi, cl13.bi);
				for (ix = x1; ix != x2; ix ++)
				{
					ptarget->SetXY(ix, l13.ai, fromf3(cllr.bi));
					cllr.next();
				}
				l12.next();
				l13.next();
				cl12.next();
				cl13.next();
			}
			else
			{
				cllr.set(x2, x1, cl13.bi, cl12.bi);
				for (ix = x2; ix != x1; ix++)
				{
					ptarget->SetXY(ix, l13.ai, fromf3(cllr.bi));
					cllr.next();
				}
				l12.next();
				l13.next();
				cl12.next();
				cl13.next();
			}
			
		}
		
		TLerper<float3> cl23(p2.y, p3.y, tof3(p2.cl), tof3(p3.cl));
		TLerper<float> l23(p2.y, p3.y, p2.x, p3.x);

		for (int h = 0; h < l23.count(); h++)
		{
			x1 = (int)l23.bi;
			x2 = (int)l13.bi;
			if (x1 < x2)
			{
				cllr.set(x1, x2, cl23.bi, cl13.bi);
				for (ix = x1; ix != x2; ix ++)
				{
					ptarget->SetXY(ix, l13.ai, fromf3(cllr.bi));
					cllr.next();
				}
				l23.next();
				l13.next();
				cl23.next();
				cl13.next();
			}
			else
			{
				cllr.set(x2, x1, cl13.bi, cl23.bi);
				for (ix = x2; ix != x1; ix++)
				{
					ptarget->SetXY(ix, l13.ai, fromf3(cllr.bi));
					cllr.next();
				}
				l23.next();
				l13.next();
				cl23.next();
				cl13.next();
			}
		}
	}


private:
	std::vector<BSLineDrawer>::iterator itScanLine;
};


// �ߵ������� û������
struct BSTriangleLineDrawer
{
	// ������
	std::vector<BSLineDrawer> Lines;

	std::vector<BSLineDrawer>::iterator itLine;

	ltri2di m_tri;
	largb m_cl1;
	dword state;

	BSTriangleLineDrawer(const ltri2di& tri, largb cl1)
		: m_tri(tri), m_cl1(cl1)
	{

	}

	BSTriangleLineDrawer(const lint2& p1, const lint2& p2,  const lint2& p3, largb cl)
		:m_tri(p1, p2, p3), m_cl1(cl)
	{

	}

	void CreatePoints()
	{
		BSPointDrawer p1(m_tri.p1, m_cl1);
		BSPointDrawer p2(m_tri.p2, m_cl1);
		BSPointDrawer p3(m_tri.p3, m_cl1);
		Lines.push_back(BSLineDrawer(p1, p2));
		Lines.push_back(BSLineDrawer(p2, p3));
		Lines.push_back(BSLineDrawer(p3, p1));
		Lines[0].state = state;
		Lines[1].state = state;
		Lines[2].state = state;

		Lines[0].CreatePoints();
		Lines[1].CreatePoints();
		Lines[2].CreatePoints();

	}

	// �������ص�
	bool BegPoint()
	{
		itLine = Lines.begin();
		if (itLine != Lines.end())
		{
			itLine->BegPoint();
			return true;
		}
		return false;
	}

	BSPointDrawer* NextPoint()
	{
		BSPointDrawer* p = itLine->NextPoint();
		if (p)
		{
			return p;
		}
		else
		{
			itLine++;
			// ���һֱ�ǿ��� �ҵ�
			while (itLine != Lines.end() && (!itLine->BegPoint()))
			{
				itLine++;
			}

			if (itLine == Lines.end())
			{
				return nullptr;
			}
			else
			{
				return itLine->NextPoint();
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
};

// ��һ���㷨����Ļ���ص� ���������Ƿ����������� ����ڣ��ͼ���Z��������Ȳ��� ���ͨ����
// �ٽ��м�������

// ��������ÿ�������ε�ÿ���㣬��Ҫ����һ���ж�

// ԭʼ�����������ι�դ�� ÿ��������ÿ���㣬���е��� ���ƵĻ���