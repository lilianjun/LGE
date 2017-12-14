#include "LCanvas.h"

bool LCanvas::Create(HWND hwnd, int w, int h)
{
	assert(w > 0);
	assert(h > 0);
	backBuffer = new LImageBuffer(w, h);

	hRenderWnd = hwnd;
	bufferWidth = w;
	bufferHeight = h;

	m_BmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	m_BmpInfo.bmiHeader.biWidth = backBuffer->Width();
	m_BmpInfo.bmiHeader.biHeight = -backBuffer->Height();
	m_BmpInfo.bmiHeader.biPlanes = 1;
	m_BmpInfo.bmiHeader.biBitCount = 32;
	m_BmpInfo.bmiHeader.biCompression = BI_RGB;
	m_BmpInfo.bmiHeader.biSizeImage = 0;
	m_BmpInfo.bmiHeader.biXPelsPerMeter = 0;
	m_BmpInfo.bmiHeader.biYPelsPerMeter = 0;
	m_BmpInfo.bmiHeader.biClrUsed = 0;
	m_BmpInfo.bmiHeader.biClrImportant = 0;

	return true;
}

bool LCanvas::Present()
{
	if (!backBuffer)
	{
		return false;
	}

	m_BmpInfo.bmiHeader.biWidth = backBuffer->Width();
	m_BmpInfo.bmiHeader.biHeight = -backBuffer->Height();

	RECT rect;
	GetClientRect(hRenderWnd, &rect);
	HDC hdc = GetDC(hRenderWnd);
	StretchDIBits(hdc, 0, 0, rect.right - rect.left, rect.bottom - rect.top,
		0, 0, backBuffer->Width(), backBuffer->Height(), backBuffer->Data(),
		&m_BmpInfo, DIB_RGB_COLORS, SRCCOPY);
	return true;
}

bool LCanvas::DrawRect(const lrecti& rect, const largb& cl_fill, const largb& cl_line, int lineWidth /* =1 */)
{
	auto myrect = ClampRect(rect);
	for (int row = myrect.ymin; row < myrect.ymax; row++)
	{
		for (int col = myrect.xmin; col < myrect.xmax; col++)
		{
			//DrawPoint(row, col, cl);
			backBuffer->SetWH(row, col, cl_fill);
		}
	}
	return true;
}

bool LCanvas::DrawTriangle(const ltri2di& triangle, const largb& line_cl, const largb& fill_cl, int lineWidth /* =1 */)
{
	// 三角形填充 扫描线

	// y在中间的点分为一个平底，一个平顶三角形
	lint2 p1 = triangle.p1;
	lint2 p2 = triangle.p2;
	lint2 p3 = triangle.p3;
	// p1 p2 p3根据y值从大到小排序
	if (p1.y < p2.y)
	{
		lswap(p1, p2);
	}
	if (p2.y < p3.y)
	{
		lswap(p2, p3);
	}
	if (p1.y < p2.y)
	{
		lswap(p1, p2);
	}

	float k = (float)(p1.x - p3.x) / (float)(p1.y - p3.y);

	// 开始从尖角绘制平底三角形
	FillFB(p1.x, p1.y, p2.x, p2.y, k,  fill_cl);

	// 开始从尖角绘制平顶三角形
	FillFT(p3.x, p3.y, p2.x, p2.y, k,  fill_cl);

	// 绘制三条线
	if (lineWidth > 0)
	{
		DrawLine(triangle.line1(), line_cl, lineWidth);
		DrawLine(triangle.line2(), line_cl, lineWidth);
		DrawLine(triangle.line3(), line_cl, lineWidth);
	}


	return true;
}

lint2 LCanvas::WndToBuffer(const POINT& pos)
{
	// 
	POINT pi = pos;
	RECT wndrec;
	GetClientRect(hRenderWnd, &wndrec);
	float wndw = (float)(wndrec.right - wndrec.left);
	float wndh = (float)(wndrec.bottom - wndrec.top);
	// 窗口比例到buffer比例
	return lint2((int)((float)bufferWidth / wndw * (float)pi.x),
		         (int)((float)bufferHeight / wndh * (float)pi.y));
}

lrecti LCanvas::WndToBuffer(const RECT& wndrec)
{
	//lrecti myrect(wndrec.left, wndrec.top, wndrec.right, wndrec.bottom);
	POINT pmin = { wndrec.left, wndrec.top };
	POINT pmax = { wndrec.right, wndrec.bottom };
	lrecti myrect(WndToBuffer(pmin),
		WndToBuffer(pmax));
	lswap(myrect.ymin, myrect.ymax);
	return myrect;
}


void LCanvas::FillFB(int xt, int yt, int xb, int yb, float k,  const largb& fill_cl)
{
	float k2 = (float)(xt - xb) / (float)(yt - yb);

	float ix1f = (float)xt;
	float ix2f = (float)xt;
	int ylim = yb - 1;
	// 需要ix1 < ix2 即 k > k2
// 	if(k < k2)
// 		lswap(k, k2);
	for (int iy = yt; iy != ylim; iy--)
	{
		int ix1 = (int)ceil(ix1f);
		// x2取小的
		int ix2 = (int)floor(ix2f);
		ScanLine(iy, ix1, ix2 + 1, fill_cl);
		// 画边缘的点
		_DrawPointXEX(ix1f, iy, fill_cl);
		_DrawPointXEX(ix2f, iy, fill_cl);
		//float w1 = ix1f - (float)(ix1 - 1);
		//float w2 = (float)(ix2 + 1) - ix2f;
		//backBuffer->SetXY(ix1, iy, fill_cl, w1);
		//backBuffer->SetXY(ix2, iy, fill_cl, w2);

		ix1f -= k;
		ix2f -= k2;
	}
}

void LCanvas::FillFT(int xb, int yb, int xt, int yt, float k, const largb& fill_cl)
{
	float k2 = (float)(xt - xb) / (float)(yt - yb);

	float ix1f = (float)xb;
	float ix2f = (float)xb;
	int ylim = yt + 1;
	// 需要 ix1 < ix2
	// 即 k < k2
 	if (k > k2)
 	{
 		lswap(k, k2);
	}
	for (int iy = yb; iy != ylim; iy++)
	{
		// x1向取大的
		int ix1 = (int) ceil(ix1f);
		// x2取小的
		int ix2 = (int) floor(ix2f);
		ScanLine(iy, ix1, ix2 + 1, fill_cl);
		_DrawPointXEX(ix1f, iy, fill_cl);
		_DrawPointXEX(ix2f, iy, fill_cl);
		// 根据
// 		float w1 = ix1f - (float)(ix1 - 1);
// 		float w2  = (float)(ix2 + 1) - ix2f;
// 		backBuffer->SetXY(ix1, iy, fill_cl, w1);
// 		backBuffer->SetXY(ix2, iy, fill_cl, w2);
		ix1f += k;
		ix2f += k2;
	}
}

bool LCanvas::DrawPoint(const lint2& pos, const largb& cl, const LBrushEx& brush)
{
	int nrow = brush.size();
	int ncol = brush[0].size();
	int halfrow = nrow / 2;
	int halfcol = ncol / 2;
	int irow = 0;
	int icol = 0;
	for (auto row : brush)
	{
		icol = 0;
		for (auto wei : row)
		{
			if (lequal(wei, 0.f))
			{
				continue;
			}
			else
			{
				backBuffer->SetXY(pos.x + icol - halfrow, pos.y + irow - halfrow,
					cl, wei);
			}
			icol++;
		}
		irow++;
	}
	return true;
}

bool LCanvas::DrawLine(const lline2di& line, const largb& cl, int linewWidth /*= 1*/)
{
	//
	lint2 p1 = line.p1;
	lint2 p2 = line.p2;

	int dx = p2.x - p1.x;
	int dy = p2.y - p1.y;


	if (abs(dx) > abs(dy))
	{
		float kx = (float)dy / dx;
		float iyf = (float)p1.y;
		int sd = p2.x > p1.x ? 1 : -1;
		kx *= sd;
		for (int ix = p1.x; ix != p2.x; ix += sd)
		{
			_DrawPointY(ix, iyf, cl, linewWidth);
			iyf += kx;
		}
	}
	else
	{
		float ky = (float)dx / dy;
		float ixf = (float)p1.x;
		int sd = p2.y > p1.y ? 1 : -1;
		ky *= sd;
		for (int iy = p1.y; iy != p2.y; iy += sd)
		{
			_DrawPointX(ixf, iy, cl, linewWidth);
			ixf += ky;
		}
	}


	return true;
}

bool LCanvas::DrawLine(const lline2di& line, const largb& cl, const LBrushEx& brush)
{
	//
	lint2 p1 = line.p1;
	lint2 p2 = line.p2;

	int dx = p2.x - p1.x;
	int dy = p2.y - p1.y;


	if (abs(dx) > abs(dy))
	{
		float kx = (float)dy / dx;
		float iyf = p1.y;
		int sd = p2.x > p1.x ? 1 : -1;
		kx *= sd;
		for (int ix = p1.x; ix != p2.x; ix += sd)
		{
			int iy = (int)roundf(iyf);
			this->DrawPoint(lint2(ix, iy), cl, brush);

			iyf += kx;
		}
	}
	else
	{
		float ky = (float)dx / dy;
		float ixf = (float)p1.x;
		int sd = p2.y > p1.y ? 1 : -1;
		ky *= sd;
		for (int iy = p1.y; iy != p2.y; iy += sd)
		{
			int ix = (int)roundf(ixf);
			this->DrawPoint(lint2(ix, iy), cl, brush);
			ixf += ky;

		}
	}

	return true;
}

void LCanvas::DrawLineBresenham(const lline2di& line, unsigned int cl, int linewWidth /*= 1*/)
{
	//

	const lint2& p1 = line.p1;
	const lint2& p2 = line.p2;

	int dx = p2.x - p1.x;
	int dy = p2.y - p1.y;
	int ux = ((dx > 0) << 1) - 1;//x的增量方向，取或-1
	int uy = ((dy > 0) << 1) - 1;//y的增量方向，取或-1
	int x = p1.x, y = p1.y, eps;//eps为累加误差

	eps = 0;dx = abs(dx); dy = abs(dy);
	if (dx > dy)
	{
		for (x = p1.x; x != p2.x; x += ux)
		{
			//backBuffer->Set(x, y, cl);
			this->DrawPoint(lint2(x, y), cl, linewWidth);
			eps += dy;
			if ((eps << 1) >= dx)
			{
				y += uy; eps -= dx;
			}
		}
	}
	else
	{
		for (y = p1.y; y != p2.y; y += uy)
		{
			//backBuffer->Set(x, y, cl);
			this->DrawPoint(lint2(x, y), cl, linewWidth);
			eps += dx;
			if ((eps << 1) >= dy)
			{
				x += ux; eps -= dy;
			}
		}
	}

	return ;
}

