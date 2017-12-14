#pragma once

//#include "LCanvas.h"
#include <windows.h>
#include "LTriDrawer.h"
#include "LSurface.h"
#include "LCircleDrawer.h"
#include "LRectDrawer.h"
#include "LTimer.h"
#include "LVertexTransformer.h"
#include "LLighter.h"
#include "LCuller.h"
#include <list>
#include <set>

// namespace


// 光栅化后的像素点
struct stPixel
{
	lint2 xy; float z; largb cl;
};

class LSoftGraph
{
public:
	// 颜色绘制的缓存
	TSurface<largb> tarBuf;
	// 深度缓存
	TSurface<float> depBuf;
	// 目标窗口
	HWND m_hWnd;
	// 颜色缓存和深度缓存的长宽
	int m_width, m_height;
	// 绘制的状态
	stDrawState m_state;
	// 计时器
	CTimer m_timer;
	dword m_dwstate;
	// 材质
	LMaterial m_mtl;

	// 顶点变换器
	LVertexTransformer m_verTrans;
	// 裁剪器
	LCuller m_culler;
	// 光照器
	LLighter m_lighter;

	//---------------------------
	//顶点源信息
	// 顶点数量
	uint32 m_nVer;
	// 顶点源数据 一一对应
	const float3* m_srcVer;
	// 顶点的源法线
	const float3* m_srcNor;
	// 顶点的源颜色
	const largb* m_srcCl;
	// 顶点的源uv坐标
	const float2* m_srcUV;

	// --------------------------
	//三角形源信息
	// 三角形数量
	uint32 m_nTri;
	// 三角形索引
	const lint3* m_srcTri;
	// 需要渲染的三角形列队 存储裁剪之后的三角形
	std::list<uint32> m_tarTriList;
	
	// ----------------------------
	// 【处理后的顶点信息】
	// view空间的点
	std::vector<float3> m_verV;
	// 目标顶点索引
	std::set<uint32> m_tarVerSet;
	// 目标顶点颜色
	std::vector<largb> m_tarCl;
	// 目标顶点法线
	std::vector<float3> m_tarNor;

	// 投影、裁剪后的额外的点
	std::vector<stVPosCl> m_verProj;

	// 投影、裁剪后的额外的三角形
	std::vector<lint3> m_triProj;

	//  需要光栅化的BUFFER空间中的三角形列表
	std::vector<BSTriDrawer> m_bsTri;

	LSoftGraph()
	{ }

	bool Create(HWND hwnd, int w, int h);


	void SetTexture();

	void SetVertexBuffer(const float3* pver, uint32 nver)
	{
		m_srcVer = pver;
		m_nVer = nver;
	}

	void SetIndexBuffer(const lint3* pindex, uint32 ntri)
	{
		m_srcTri = pindex;
		m_nTri = ntri;
	}

	void SetColorBuffer(const largb* pcl)
	{
		m_srcCl = pcl;
	}

	void SetUVBuffer(const float2* puv)
	{
		m_srcUV = puv;
	}

	void SetNorBuffer(const float3* pnor)
	{
		m_srcNor = pnor;
	}

	// 绘制网格
	void DrawIndexTriangle(const stDrawState& ds);

	void SetWorld(const float3& pos = float3(0.f, 0.f, 0.f),
		const float3& rot = float3(0.f, 0.f, 0.f),
		const float3& scl = float3(1.f, 1.f, 1.f))
	{
		m_verTrans.SetWorld(pos, rot, scl);
	}

	void SetView(const float3& pos, const float3& up, const float3& dir)
	{
		m_verTrans.SetView(pos, up, dir);
	}

	void SetProj(float fov, float _aspect, float Znear, float Zfar)
	{
		m_verTrans.SetProj(fov, _aspect, Znear, Zfar);
	}

	// 点的坐标的转换
	BSPointDrawer FromWnd(const POINT& p, dword cl = 0);

	lint2 FromWndEX(const POINT& p);

	POINT ToWnd(const lint2& p);

	// 窗口空间的线转换到BUFFER空间
	BSLineDrawer FromWnd(const POINT& p1, const POINT& p2, dword cl = 0)
	{
		return BSLineDrawer(FromWnd(p1, cl), FromWnd(p2, cl));
	}

	// 窗口空间的三角形转换到buffer空间
	BSTriDrawer FromWnd(const POINT& p1, const POINT& p2, const POINT& p3, dword cl = 0)
	{
		return BSTriDrawer(FromWnd(p1, cl), FromWnd(p2, cl), FromWnd(p3,cl));
	}

	// 窗口空间矩形到buffer空间
	lrecti FromWndEX(const RECT& rect)
	{
		POINT pmin = { rect.left, rect.bottom };
		POINT pmax = { rect.right, rect.top };
		return lrecti(FromWndEX(pmin), FromWndEX(pmax));
	}

	// 绘制窗口对象
	bool DrawWndPoint(const POINT& p, dword cl);

	bool DrawWndLine(const POINT& p1, const POINT& p2, dword cl);

	bool DrawWndLine(const POINT& p1, dword cl1,  const POINT& p2, dword cl2);

	bool DrawWndTri(const POINT& p1, const POINT& p2, const POINT& p3, largb cl_line, largb cl_fill, bool bFill)
	{
		BSTriDrawer bst = FromWnd(p1, p2, p3, cl_fill);
		bst.CreatePoints();
		bst.DrawTo(&tarBuf, &depBuf);
		return true;
	}

	bool DrawWndTri(const POINT& p1, largb cl1, const POINT& p2, largb cl2, const POINT& p3, largb cl3);

	bool DrawWndTriL(const POINT& p1,  const POINT& p2, const POINT& p3, largb cl);

	// 画窗口空间的圆
	bool DrawWndCircle(const POINT& center, const POINT& p, largb cl1, largb cl2);

	bool DrawWndCircleL(const POINT& center, const POINT& p, largb cl1);

	// 绘制buffer空间的2D点
	void DrawBSPoint(const lint2& p, largb cl)
	{
		BSPointDrawer pd(p.x, p.y, cl);
		pd.DrawTo(&tarBuf, &depBuf);
	}

	// 绘制buffer空间的2D的线
	void DrawBSLine(const lline2di& line, largb cl);

	// 绘制buffer空间的2D三角形
	void DrawBSTri(const ltri2di& tri, largb cl)
	{

	}

	// 绘制buffer空间的圆形
	void DrawBSCircle(const lcircle2di& circle, largb cl);

	void DrawWndRect(const RECT& rect, largb cl1, largb cl2, largb cl3, largb cl4);

	void DrawWndRectL(const RECT& rect, largb cl)
	{
		lrecti myrect = FromWndEX(rect);
		BSRectDrawerL rd(myrect, cl);
		rd.CreatePoints();
		rd.DrawTo(&tarBuf, &depBuf);
	}
	
	void ClearColor(largb cl = largb::gray(1.f))
	{
		tarBuf.Clear(cl);
	}

	void ClearDepth(float f = 1.0f)
	{
		depBuf.Clear(f);
	}

	void UpdateToWnd();
};