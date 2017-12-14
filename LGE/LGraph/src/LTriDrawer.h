#pragma once

#include "lmath.hpp"

#include "LSurface.h"
#include "LLineDrawer.h"

// 顶点流水线
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

// 像素流水线
template <typename t_prim_list_drawer>
class lpixel_pipeline
{
	t_prim_list_drawer prim_list_drawer;
	bool operator()();
};


// 组件组合，一个个泛型的组件，最终组合成了渲染器。从小到大的任何部分都可以替换
// 全部都是泛型的。

// 只有位置 
struct stVPos
{
	float3 pos;
};

// 有颜色值 颜色值来自于顶点diffuse 
struct stVPosCl
{
	float3 pos; largb cl;
};


// 采样器
struct stSampler
{
	largb operator()(const float2& uv)
	{ }
};

// 像素阶段的 顶点属性
struct stPSInput
{
	float z; largb cl;

	// 光栅化中，一个需要用到插值，插值得到属性值

	// 一个要用到颜色，需要返回颜色
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

// 像素着色器
// 输入泛型的stPSInput 输出color
// 有不同的结构，不同的算法
struct stPixelShader
{
	template <typename TPro>
	largb operator()(const float2& p, const TPro& pro)
	{
		std::cout << "像素着色器开始处理点和属性" << std::endl;
		return pro.cl;
	}
};

// 提供一个泛型的三角形绘制器 插值三角形的各属性值
// TProperty是顶点的属性 需要提供：operator + *float 用于插值
// 或者用一个重载的lerp函数？
// 第二点，需要重载operator(const float2&) 接收像素点的位置信息
template<typename TProperty>
struct LTriDrawer
{
	float2 p1, p2, p3;
	TProperty pro1, pro2, pro3;

	template <typename TShader>
	void Draw(TShader sd)
	{
		// 重新排序 Y从小到大
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

		// 旋转初始两条线
		TLerper<float>* lineL, *lineR;
		TLerper<TProperty>* proL, *proR;
		TProperty ipro;
		float2 ipoint;
		while (true)
		{
			// 退出条件
			// 切换线条件
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

// 定义顶点着色器

// 再提供一个函数对象，相当于像素着色器的东西，定义输入输出
// 重载operator()


// 三角形绘制器 绘制经投影变换、视口裁剪后的三角形
struct LTriDrawerX
{
	stVPosCl ver1, ver2, ver3;

	// 视口转换处理
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

// BS空间的三角形绘制对象
class BSTriDrawer
{
public:
	// 三个点
	BSPointDrawer p1, p2, p3;

	// 三条线
	BSLineDrawer line12;
	BSLineDrawer line23;
	BSLineDrawer line13;

	// 2d三角形
	ltri2di triangle;

	// 矩形范围
	lrecti rect;
	
	// 计算重心坐标的分母
	int denominator;

	// 渲染状态
	dword state;

	BSTriDrawer()
	{}

	BSTriDrawer(const BSPointDrawer& _p1, const BSPointDrawer& _p2, const BSPointDrawer& _p3)
		:p1(_p1), p2(_p2), p3(_p3), 
		triangle(lint2(_p1.x, _p1.y), lint2(_p2.x, _p2.y), lint2(_p3.x, _p3.y))
	{
		rect = triangle.rect();
	}

	// 禁止复制构造
	BSTriDrawer(const BSTriDrawer&) = delete;

	// 禁止复制赋值
	BSTriDrawer& operator = (const BSTriDrawer&) = delete;

	// 转移构造
	BSTriDrawer(const BSTriDrawer&& v )
	{ }

	// 转移赋值
	BSTriDrawer& operator = (const BSTriDrawer&& v)
	{
		return *this;
	}

	// 扫描线 三角形的像素点存储于扫描线中
	// 扫描线是y从低到高排列的
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

	// 创建三角形的像素点
	void CreatePoints()
	{
		// 重新排序 Y从小到大
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

		// 从小到大 从左到右 从下到上
		// 两种情况：p1下 p2左 p3上 
		// p1下 p2右 p3 上

		// 创建三条边
		line13.Set(p1, p3);
		line12.Set(p1, p2);
		line23.Set(p2, p3);
		line12.state = state;
		line23.state = state;
		line13.state = state;
		line12.CreatePointY();
		line23.CreatePointY();
		line13.CreatePointY();

		// 扫描线的数量为高度 也为长边的像素数量
		scanLines.resize(line13.PointCount());

		// 开始下半个平顶三角形
		line12.BegPoint();
		line13.BegPoint();
		// 12线上的点
		BSPointDrawer* p12 = line12.NextPoint();
		// 13线上的点
		BSPointDrawer* p13 = line13.NextPoint();
		int nline = 0;
		while (p12)
		{
			// 从左到右
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
		// 到这里应该到了p2点水平线
		
		// 开始上半个平顶三角形
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

		// 计算颜色
		//CreateColors();
	}


	// 三角形的第二种创建方式 颜色值也是进行递推
	void CreatePointsEX()
	{
		// 重新排序 Y从小到大
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

		// 扫描线的数量为高度 也为长边的像素数量
		scanLines.resize(line13.PointCount());

		// 开始上半个平底三角形
		line12.BegPoint();
		line13.BegPoint();
		// 12线上的点
		BSPointDrawer* p12 = line12.NextPoint();
		// 13线上的点
		BSPointDrawer* p13 = line13.NextPoint();
		int nline = 0;
		while (p12)
		{
			// 从左到右
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
		// 到这里应该到了p2点水平线

		// 开始下半个平底三角形
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

	// 计算重心坐标
	float3 GetWeightCoord(int x, int y)
	{
		float w1 = (_getw1(x, y));
		float w2 = (_getw2(x, y));
		float w3 = (_getw3(x, y));
		return float3(w1, w2, w3).getclamp(0.f, 1.f);
	}

	// 计算三角形各像素点的颜色
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

		// 如何插值获取?
		// 跟三个顶点的距离进行权重
		// 重心坐标
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

	// 计算各像素点的UV坐标
	// 两种方法，方法一，在计算点时，递推。方法二：用重心坐标
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

	// 遍历像素点
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
			// 如果一直是空行 找到
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
		// 重新排序 Y从小到大
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
			// 从左到右
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


// 线的三角形 没有填充的
struct BSTriangleLineDrawer
{
	// 三条线
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

	// 遍历像素点
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
			// 如果一直是空行 找到
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
};

// 另一种算法：屏幕像素点 逐个计算点是否在三角形内 如果在，就计算Z，进行深度测试 如果通过，
// 再进行计算其他

// 计算量：每个三角形的每个点，需要进行一次判断

// 原始方法：三角形光栅化 每个三角形每个点，进行递推 递推的话，