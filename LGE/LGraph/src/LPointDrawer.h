#pragma once

#include <math.h>
#include "LSurface.h"
#include <assert.h>

enum ERenderState
{
	eRSNull = 0,
	eColor = 1,
	eTex = 1 << 1,
	eDepthTest = 1 << 2,
	eDepthWrite = 1 << 3,
	eLighting = 1 << 4,
	eLightingPixel = 1 << 5
};

// 绘制参数
struct stDrawState
{
	// 是否写入深度缓冲
	bool bZWrite;
	// 是否进行深度测试
	bool bZTest;
	// 是否绘制贴图
	bool bTexture;
	// 是否开启光照
	bool bLight;
	// 绘制实体还是线框
	bool bFill;

	// 顶点是否有法线
	bool bVertexNormal;
	// 是否有顶点cl
	bool bVertexDiffuse;

	dword dw()const
	{
		dword s = ERenderState::eColor;
		return   s |
			(bTexture ? ERenderState::eTex : 0) |
			(bZTest ? ERenderState::eDepthTest : 0) |
			(bZWrite ? ERenderState::eDepthWrite : 0) ;

	}

};

struct LMaterial
{
	largb diffuse;
};



// buffer space 空间的点
class  BSPointDrawer
{
public:
	// buffer空间位置
	union 
	{
		struct  {	int x, y;};
		lint2 xy;
	};

	// 贴图的UV坐标
	union 
	{
		struct { float u, v; };
		float2 uv;
	};

	// 颜色值
	largb cl;

	// 深度值
	float z;
	
	// 所对应的贴图
	const LColorBuffer* ptex;

	BSPointDrawer()
		: x(-1), y(-1), u(-1), v(-1), z(1.f), cl(0), ptex(nullptr)
	{
	}

	// 以颜色点初始化
	BSPointDrawer(int _x, int _y, largb _cl = 0, float _z = 1.f)
		:x(_x), y(_y), cl(_cl), z(_z),u(-1), v(-1), ptex(nullptr)
	{
	}

	BSPointDrawer(const lint2& _p, largb _cl, float _z = 1.f)
		:x(_p.x), y(_p.y), cl(_cl)
	{

	}


	// 带贴图的点初始化
	BSPointDrawer(const lint2& _xy, const float2& _uv = float2(-1.f, -1.f), const LColorBuffer* _tex = nullptr, float _z = 1.f)
		:xy(_xy), uv(_uv), cl(0), ptex(_tex), z(_z)
	{ }

	BSPointDrawer(const BSPointDrawer& p)
		:xy(p.xy), uv(p.uv), cl(p.cl), z(p.z), ptex(p.ptex)
	{ }

	BSPointDrawer& operator =(const BSPointDrawer& p)
	{
		xy = p.xy; uv = p.uv;
		z = p.z; ptex = p.ptex;
		cl = p.cl;
		return *this;
	}

	void Set(const lint2& p, largb _cl)
	{
		x = p.x; y = p.y;  cl = _cl;
	}

	// 绘制 超出范围 就不绘制
	bool DrawTo(LColorBuffer* ptarget, LDepthBuffer* pdethbuf)
	{
		assert(ptarget);
		if (x > ptarget->GetW() - 1 || x < 0 || y > ptarget->GetH() - 1 || y < 0)
		{
			return false;
		}
		ptarget->SetXY(x, y, cl);
		return true;
	}
};

// 线的插值器
struct LerperInt 
{
	// (a1, b1为起点) (a2、 b2)为终点
	// a为自变量 每次加一 b为因变量 根据效率变化 
	// 用接口nextB递推
	LerperInt(int a1, int b1, int a2, int b2, int da = 1)
		:m_a1(a1), m_a2(a2), m_b1(b1), m_b2(b2), m_da(da)
	{
		m_s = a2 > a1 ? 1 : -1;
		m_da *= m_s;
		// 斜率
		m_db = (float)(b2 - b1) / (float)(a2 - a1) * m_da;
		ai = a1;
		bi = b1;
		m_bi = (float)b1;
	}

	inline void next()
	{
		ai += m_da;
		m_bi += m_db;
		bi = (int)m_bi;
	}

	inline bool end()
	{
		return  ai == m_a2;
	}

	// 获取数量
	inline int count()
	{
		return abs(m_a2 - m_a1);
	}

	int ai, bi;

private:
	int m_da;
	float m_db, m_bi;
	int m_a1, m_a2, m_b1, m_b2;
	int m_s;
};



// 不定参数的模板？
template <typename T>
struct TLerper
{
	TLerper() {}

	TLerper(int a1,  int a2, const T& b1, const T& b2, int da = 1)
		:m_a1(a1), m_a2(a2), m_da(da)
	{
		m_s = a2 > a1 ? 1 : -1;
		m_da *= m_s;
		// 因变量的每次变化量 T类型需要重载 * 和 + -操作符
		float k = ((float)m_da / (float)(a2 - a1));
		m_db = (b2 - b1) * k;
		ai = a1;
		bi = b1;
	}

	inline void set(int a1, int a2, const T& b1, const T& b2, int da = 1)
	{
		m_a1 = (a1); m_a2 = (a2); m_da = (da);
		m_s = a2 > a1 ? 1 : -1;
		m_da *= m_s;
		// 斜率
		m_db = (b2 - b1) / (float)(a2 - a1) * (float)m_da;
		ai = a1;
		bi = b1;
	}

	inline void next()
	{
		ai += m_da;
		bi += m_db;
	}

	inline bool end()
	{
		return  ai == m_a2;
	}

	// 获取数量
	inline int count()
	{
		return abs(m_a2 - m_a1);
	}

	int ai; T bi;
private:
	int m_da;
	T m_db;
	int m_a1, m_a2;// m_b1, m_b2;
	int m_s;
};

// X主导的Bresenham的画线方式
struct LLineBreX
{
	int dx, dy, ux, uy, eps;

	LLineBreX(int x1, int y1, int x2, int y2)
	{
		dx = x2 - x1;
		dy = y2 - y1;
		ux = ((dx > 0) << 1) - 1;//x的增量方向，取或-1
		uy = ((dy > 0) << 1) - 1;//y的增量方向，取或-1

		eps = 0;
		dx = abs(dx);
		dy = abs(dy);
		assert(dx > dy);
	}

	inline void P(int& ix, int& iy)
	{
		eps += dy;
		if ((eps << 1) >= dx)
		{
			iy += uy; eps -= dx;
		}
		ix += ux;
	}
};

// Y主导的画线方式
struct LLineBreY
{
	int dx, dy, ux, uy, eps;

	LLineBreY(int x1, int y1, int x2, int y2)
	{
		dx = x2 - x1;
		dy = y2 - y1;
		ux = ((dx > 0) << 1) - 1;//x的增量方向，取或-1
		uy = ((dy > 0) << 1) - 1;//y的增量方向，取或-1

		eps = 0;
		dx = abs(dx);
		dy = abs(dy);
		assert(dy > dx);
	}

	inline void P(int& ix, int& iy)
	{
		eps += dx;
		if ((eps << 1) >= dy)
		{
			ix += ux; eps -= dy;
		}
		iy += uy;
	}
};