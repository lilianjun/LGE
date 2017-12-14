#include "LSoftGraph.h"
#include <iostream>

bool LSoftGraph::Create(HWND hwnd, int w, int h)
{
	m_hWnd = hwnd;
	m_width = w;
	m_height = h;
	tarBuf.Create(w, h);
	depBuf.Create(w, h);
	m_verTrans.w = w;
	m_verTrans.h = h;
	return true;
}

void LSoftGraph::DrawIndexTriangle(const stDrawState& ds)
{
	m_state = ds;
	m_dwstate = ds.dw();


	// 第一步【变换阶段】 所有顶点进行变换到view空间
	// 【源顶点】==》【view空间顶点】
	m_verV.resize(m_nVer);
	m_verTrans.TransVerModel(m_srcVer, m_nVer, m_verV.data());

	// 第二步【裁剪阶段】所有三角形进行视锥体裁剪
	// 【源三角形列表】 ==》 【可见三角形列表1】
	m_tarTriList.clear();
	for (uint32 i = 0; i < m_nTri; i++)
	{
		const auto& i3 = m_srcTri[i];
		if (m_culler.CullTri(m_verV[i3.x], m_verV[i3.y], m_verV[i3.z]))
		{
			m_tarTriList.push_back(i);
		}
	}

	// 第三步：将三角形列表进行深度排序、遮挡剔除
	// 【可见三角形列表1】 ==>【可见三角形列表2】

	// 第四步 得到最终需要渲染的三角形索引，根据索引得到三角形顶点
	// 创建【目标顶点列表】
	m_tarVerSet.clear();
	for (const auto& itri : m_tarTriList)
	{
		const lint3& i3 = m_srcTri[itri];
		m_tarVerSet.insert(i3.x);
		m_tarVerSet.insert(i3.y);
		m_tarVerSet.insert(i3.z);
	}

	// 第五步  
	// 【光照阶段】 将【目标顶点】的法线转换到view空间
	if (m_state.bLight)
	{
		m_tarCl.assign(m_srcCl, m_srcCl + m_nVer);
		m_tarNor.resize(m_nVer);
		float3 tempNor;
		for (const auto& iver : m_tarVerSet)
		{
			//m_tarNor[iver] = m_verTrans.TransNorModel(m_srcNor[iver]);
			tempNor = m_verTrans.TransNorModel(m_srcNor[iver]);
			m_tarCl[iver] = m_lighter.LightVertex(m_verV[iver], tempNor,
				m_state.bVertexDiffuse ? m_srcCl[iver] : m_mtl.diffuse);
			m_tarNor[iver] = tempNor;
		}
	}

	// 【投影阶段】 把view空间的点转换到buffer空间
	for (const auto& iver : m_tarVerSet)
	{
		m_verV[iver] = m_verTrans.TransPointProj(m_verV[iver]);
	}

	// 裁剪阶段 把在投影面范围以外的三角形，裁剪

	// 将所有的点转换到buffer空间？不用。

	// 绘制的时候，指定目标的w h，再绘制。这样可以灵活地所以绘制

	// 第六步 【光栅化阶段】
	//m_bsTri.resize(m_tarTriList.size());
	int i = 0;
	for (const auto& itri : m_tarTriList)
	{
		const lint3& i3 = m_srcTri[itri];

		std::vector<stVPosCl> trivers{ stVPosCl{ (m_verV[i3.x]) }, 
			stVPosCl{ (m_verV[i3.y]) },
			stVPosCl{ (m_verV[i3.z]) } };

		
		if (m_state.bLight)
		{
			trivers[0].cl = m_tarCl[i3.x];
			trivers[1].cl = m_tarCl[i3.y];
			trivers[2].cl = m_tarCl[i3.z];
		}
		else
		{
			if (m_state.bVertexDiffuse)
			{
				trivers[0].cl = m_srcCl[i3.x];
				trivers[1].cl = m_srcCl[i3.y];
				trivers[2].cl = m_srcCl[i3.z];
			}
		}
		

		
		auto tris = m_culler.CullProjTri( trivers );

		// 如果经裁剪，没有三角形，就抛弃
		if (tris.size() < 3)
		{
			continue;
		}
		else if (tris.size() == 3)
		{
			//LTriDrawer{ tris[0], tris[1], tris[2] }.DrawTo((dword*)tarBuf.Data(), tarBuf.GetW(), tarBuf.GetH());
			lmake_tridrawer(tris[0].pos.xy(), tris[1].pos.xy(), tris[2].pos.xy(), 1.f, 1.f, 1.f);
		}
		else
		{
			for (int i = 1; i < tris.size(); i++)
			{
				int i2 = (i + 1 == tris.size() ? 0 : i+1);
				//LTriDrawer{ tris[0], tris[i], tris[i2]  }.DrawTo((dword*)tarBuf.Data(), tarBuf.GetW(), tarBuf.GetH());
			}
		}
		
		i++;
	}
}

BSPointDrawer LSoftGraph::FromWnd(const POINT& p, dword cl /*= 0*/)
{
	RECT rect;
	GetClientRect(m_hWnd, &rect);
	float wndW = rect.right - rect.left;
	float wndH = rect.bottom - rect.top;
	float myW = p.x *  m_width / wndW;
	float myH = m_height - p.y * m_height / wndH;
	return BSPointDrawer((int)roundf(myW), (int)roundf(myH), cl);
}

lint2 LSoftGraph::FromWndEX(const POINT& p)
{
	RECT rect;
	GetClientRect(m_hWnd, &rect);
	float wndW = (float)(rect.right - rect.left);
	float wndH = (float)(rect.bottom - rect.top);
	float myX = p.x *  m_width / wndW;
	float myY = m_height - p.y * m_height / wndH;
	return lint2((int)roundf(myX), (int)roundf(myY));
}

bool LSoftGraph::DrawWndPoint(const POINT& p, dword cl)
{
	BSPointDrawer bsp = FromWnd(p, cl);
	bsp.DrawTo(&tarBuf, &depBuf);
	return true;
}

bool LSoftGraph::DrawWndLine(const POINT& p1, const POINT& p2, dword cl)
{
	BSLineDrawer bsl = FromWnd(p1, p2, cl);
	bsl.p1.cl = cl;
	bsl.CreatePoints();
	bsl.DrawTo(&tarBuf, &depBuf);
	return true;
}

bool LSoftGraph::DrawWndLine(const POINT& p1, dword cl1, const POINT& p2, dword cl2)
{
	BSLineDrawer bsl = FromWnd(p1, p2);
	bsl.p1.cl = cl1;
	bsl.p2.cl = cl2;
	bsl.state = ERenderState::eColor;
	m_timer.Start();
	for (int i = 0; i < 1000; i++)
	{
		bsl.CreatePoints();
	}
	auto t1 = m_timer.Stop();
	std::cout << "line method 1: " << t1 << std::endl;
	//this->UpdateToWnd();


	m_timer.Start();
	for (int i = 0; i < 10000; i++)
	{
		bsl.CreatePointsEX();
	}
	auto t2 = m_timer.Stop();
	std::cout << "line method 2: " << t2 << "times:" << t2 / t1 << std::endl;

	bsl.DrawTo(&tarBuf, &depBuf);
	return true;
}

bool LSoftGraph::DrawWndTri(const POINT& p1, largb cl1, const POINT& p2, largb cl2, const POINT& p3, largb cl3)
{
	BSTriDrawer bst = FromWnd(p1, p2, p3);
	bst.p1.cl = cl1;
	bst.p2.cl = cl2;
	bst.p3.cl = cl3;
	bst.state = eColor;
	m_timer.Start();
	bst.CreatePoints();
	auto t1 = m_timer.Stop();
	std::cout << "画三角形【方法一】用时" << t1 << std::endl;

	m_timer.Start();
	bst.CreatePointsEX();
	auto t2 = m_timer.Stop();
	std::cout << "画三角形【方法二】用时" << m_timer.Stop() << " 相差倍数 " << t1 / t2 << std::endl;

	bst.DrawTo(&tarBuf, &depBuf);
	return true;
}

bool LSoftGraph::DrawWndTriL(const POINT& p1, const POINT& p2, const POINT& p3, largb cl)
{
	BSTriangleLineDrawer bst(ltri2di(FromWndEX(p1), FromWndEX(p2), FromWndEX(p3)), cl);
	bst.state = eColor;
	bst.CreatePoints();
	bst.DrawTo(&tarBuf, &depBuf);
	return true;
}

bool LSoftGraph::DrawWndCircle(const POINT& center, const POINT& p, largb cl1, largb cl2)
{
	lint2 pcen = FromWndEX(center);
	lint2 pp = FromWndEX(p);
	int r = pcen.distance(pp);
	lcircle2di circle(pcen, r);
	BSCircleDrawer cd(circle, cl1, cl2);
	cd.CreatePoints();
	cd.DrawTo(&tarBuf, &depBuf);
	return true;
}

bool LSoftGraph::DrawWndCircleL(const POINT& center, const POINT& p, largb cl1)
{
	lint2 pcen = FromWndEX(center);
	lint2 pp = FromWndEX(p);
	int r = pcen.distance(pp);
	lcircle2di circle(pcen, r);
	BSCircleLineDrawer cd(circle, cl1);
	cd.CreatePoints();
	cd.DrawTo(&tarBuf, &depBuf);
	return true;
}

void LSoftGraph::DrawBSLine(const lline2di& line, largb cl)
{
	BSLineDrawer td;
	td.p1.x = line.p1.x;
	td.p1.y = line.p1.y;
	td.p2.x = line.p2.x;
	td.p2.y = line.p2.y;
	td.p1.cl = cl;
	td.p2.cl = cl;
	td.CreatePoints();
	td.DrawTo(&tarBuf, &depBuf);
}

void LSoftGraph::DrawWndRect(const RECT& rect, largb cl1, largb cl2, largb cl3, largb cl4)
{
	lrecti myrect = FromWndEX(rect);
	BSRectDrawer rd(myrect, cl1, cl2, cl3, cl4);
	rd.CreatePoints();
	rd.DrawTo(&tarBuf, &depBuf);
}

void LSoftGraph::UpdateToWnd()
{
	BITMAPINFO m_BmpInfo;
	memset(&m_BmpInfo, sizeof(m_BmpInfo), 0);
	m_BmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	m_BmpInfo.bmiHeader.biWidth = tarBuf.GetW();
	m_BmpInfo.bmiHeader.biHeight = tarBuf.GetH();
	m_BmpInfo.bmiHeader.biPlanes = 1;
	m_BmpInfo.bmiHeader.biBitCount = 32;
	m_BmpInfo.bmiHeader.biCompression = BI_RGB;
	m_BmpInfo.bmiHeader.biSizeImage = 0;
	m_BmpInfo.bmiHeader.biXPelsPerMeter = 0;
	m_BmpInfo.bmiHeader.biYPelsPerMeter = 0;
	m_BmpInfo.bmiHeader.biClrUsed = 0;
	m_BmpInfo.bmiHeader.biClrImportant = 0;

	RECT rect;
	GetClientRect(m_hWnd, &rect);
	HDC hdc = GetDC(m_hWnd);
	StretchDIBits(hdc, 0, 0, rect.right - rect.left, rect.bottom - rect.top,
		0, 0, tarBuf.GetW(), tarBuf.GetH(), tarBuf.Data(),
		&m_BmpInfo, DIB_RGB_COLORS, SRCCOPY);
	ReleaseDC(m_hWnd, hdc);
}

POINT LSoftGraph::ToWnd(const lint2& p)
{
	RECT rect;
	GetClientRect(m_hWnd, &rect);
	float wndW = rect.right - rect.left;
	float wndH = rect.bottom - rect.top;
	float myW = p.x *  wndW / m_width;
	float myH = wndH - p.y * wndH / m_height;
	return { (int)roundf(myW), (int)roundf(myH) };
}

