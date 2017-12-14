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


// ��դ��������ص�
struct stPixel
{
	lint2 xy; float z; largb cl;
};

class LSoftGraph
{
public:
	// ��ɫ���ƵĻ���
	TSurface<largb> tarBuf;
	// ��Ȼ���
	TSurface<float> depBuf;
	// Ŀ�괰��
	HWND m_hWnd;
	// ��ɫ�������Ȼ���ĳ���
	int m_width, m_height;
	// ���Ƶ�״̬
	stDrawState m_state;
	// ��ʱ��
	CTimer m_timer;
	dword m_dwstate;
	// ����
	LMaterial m_mtl;

	// ����任��
	LVertexTransformer m_verTrans;
	// �ü���
	LCuller m_culler;
	// ������
	LLighter m_lighter;

	//---------------------------
	//����Դ��Ϣ
	// ��������
	uint32 m_nVer;
	// ����Դ���� һһ��Ӧ
	const float3* m_srcVer;
	// �����Դ����
	const float3* m_srcNor;
	// �����Դ��ɫ
	const largb* m_srcCl;
	// �����Դuv����
	const float2* m_srcUV;

	// --------------------------
	//������Դ��Ϣ
	// ����������
	uint32 m_nTri;
	// ����������
	const lint3* m_srcTri;
	// ��Ҫ��Ⱦ���������ж� �洢�ü�֮���������
	std::list<uint32> m_tarTriList;
	
	// ----------------------------
	// �������Ķ�����Ϣ��
	// view�ռ�ĵ�
	std::vector<float3> m_verV;
	// Ŀ�궥������
	std::set<uint32> m_tarVerSet;
	// Ŀ�궥����ɫ
	std::vector<largb> m_tarCl;
	// Ŀ�궥�㷨��
	std::vector<float3> m_tarNor;

	// ͶӰ���ü���Ķ���ĵ�
	std::vector<stVPosCl> m_verProj;

	// ͶӰ���ü���Ķ����������
	std::vector<lint3> m_triProj;

	//  ��Ҫ��դ����BUFFER�ռ��е��������б�
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

	// ��������
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

	// ��������ת��
	BSPointDrawer FromWnd(const POINT& p, dword cl = 0);

	lint2 FromWndEX(const POINT& p);

	POINT ToWnd(const lint2& p);

	// ���ڿռ����ת����BUFFER�ռ�
	BSLineDrawer FromWnd(const POINT& p1, const POINT& p2, dword cl = 0)
	{
		return BSLineDrawer(FromWnd(p1, cl), FromWnd(p2, cl));
	}

	// ���ڿռ��������ת����buffer�ռ�
	BSTriDrawer FromWnd(const POINT& p1, const POINT& p2, const POINT& p3, dword cl = 0)
	{
		return BSTriDrawer(FromWnd(p1, cl), FromWnd(p2, cl), FromWnd(p3,cl));
	}

	// ���ڿռ���ε�buffer�ռ�
	lrecti FromWndEX(const RECT& rect)
	{
		POINT pmin = { rect.left, rect.bottom };
		POINT pmax = { rect.right, rect.top };
		return lrecti(FromWndEX(pmin), FromWndEX(pmax));
	}

	// ���ƴ��ڶ���
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

	// �����ڿռ��Բ
	bool DrawWndCircle(const POINT& center, const POINT& p, largb cl1, largb cl2);

	bool DrawWndCircleL(const POINT& center, const POINT& p, largb cl1);

	// ����buffer�ռ��2D��
	void DrawBSPoint(const lint2& p, largb cl)
	{
		BSPointDrawer pd(p.x, p.y, cl);
		pd.DrawTo(&tarBuf, &depBuf);
	}

	// ����buffer�ռ��2D����
	void DrawBSLine(const lline2di& line, largb cl);

	// ����buffer�ռ��2D������
	void DrawBSTri(const ltri2di& tri, largb cl)
	{

	}

	// ����buffer�ռ��Բ��
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