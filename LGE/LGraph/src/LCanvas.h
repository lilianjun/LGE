#pragma once

#include "LImageBuffer.h"
#include "lmath.hpp"
#include <assert.h>
#include <windows.h>

#include "LTriDrawer.h"

// 画线的画刷
struct LBrush
{
	// 颜色
	largb color;
	int w, h;
	// 像素点的相对位置
	std::vector<lint2> pixels;
	// 像素点的权重
	std::vector<float> weights;

};

typedef std::vector< std::vector<float> > LBrushEx;


// 2D绘制接口
class LCanvas
{
public:
	LImageBuffer* backBuffer;
	std::vector<unsigned int> frontBuffer;

	LCanvas():backBuffer(nullptr), bufferHeight(0), bufferWidth(0), hRenderWnd(nullptr)
	{
	}

	// 创建
	bool Create(HWND hwnd, int w, int h);

	void Destroy()
	{
		if (backBuffer)
		{
			delete backBuffer;
			backBuffer = nullptr;
		}
	}

	// 翻转显示到窗口
	bool Present();


	bool Clear(const largb& cl)
	{
		return backBuffer->Clear(cl);
	}

	bool CheckX(int x)const
	{
		return x > -1 && x < backBuffer->Width();
	}

	bool CheckY(int y)const
	{
		return y > -1 && y < backBuffer->Height();
	}

	int ClampX(int x)const
	{
		return lclamp(x, 0, backBuffer->Width());
	}

	int ClampY(int y)const
	{
		return lclamp(y, 0, backBuffer->Height());
	}

	lint2 ClampPoint(const lint2& pos)const
	{
		return lint2(ClampX(pos.x), ClampY(pos.y));
	}

	// 裁剪视口外的线
	lline2di ClampLine(const lline2di& line)const;

	// 裁剪视口外的矩形
	lrecti ClampRect(const lrecti& rect)
	{
		return lrecti(ClampPoint(rect.pmin), ClampPoint(rect.pmax));
	}

	// 裁剪视口外的三角形
	ltri2di ClampTriangle(const ltri2di& tri);

	// 绘制矩形
	bool DrawRect(const lrecti& rect, const largb& cl_fill, const largb& cl_line, int lineWidth = 1 );

	// 在缓冲区上绘制点 size：点的大小 像素数量
	bool DrawPointWnd(const POINT& pos, const largb& cl, int size = 0)
	{
		return this->DrawPoint(WndToBuffer(pos), cl, size);
	}

	// 用画刷在窗口上画点
	bool DrawPointWnd(const POINT& pos, const largb& cl, const LBrushEx& brush)
	{
		return this->DrawPoint(WndToBuffer(pos), cl, brush);
	}

	// 画点
	bool DrawPoint(const lint2& pos, const largb& cl, int size = 0)
	{
		return this->DrawRect(lrecti(pos.x - size, pos.y - size, pos.x + size, pos.y + size), cl, cl);
	}

	// 用画刷画点
	bool DrawPoint(const lint2& pos, const largb& cl, const LBrushEx& brush);

	// 窗口空间画线
	bool DrawLineWnd(const POINT& p1, const POINT& p2, const largb& cl, int lineWidth = 1)
	{
		return DrawLine(lline2di(WndToBuffer(p1), WndToBuffer(p2)), cl, lineWidth);
	}

	// 窗口空间用画刷画线
	bool DrawLineWnd(const POINT& p1, const POINT& p2, const largb& cl, const LBrushEx& brush)
	{
		return DrawLine(lline2di(WndToBuffer(p1), WndToBuffer(p2)), cl, brush);
	}

	// 用画刷画线
	bool DrawLine(const lline2di& line, const largb& cl, const LBrushEx& brush);

	// 画线
	bool DrawLine(const lline2di& line, const largb& cl, int linewWidth = 1);

	// 窗口空间画三角形
	bool DrawTriangleWnd(const POINT& p1, const POINT& p2, const POINT& p3, const largb& line_cl, const largb& fill_cl, int lineWidth = 1)
	{
		return DrawTriangle( ltri2di( WndToBuffer(p1), WndToBuffer(p2), WndToBuffer(p3))
		, line_cl,  fill_cl,lineWidth);
	}

	// 画三角形
	bool DrawTriangle(const ltri2di& triangle, const largb& line_cl, const largb& fill_cl, int lineWidth = 1);

	// 画贴图三角形
	bool DrawTexturedTriangle(const ltri2di& triangle, const char* ptex, const char* puv);

	// 画3D三角形 已变换到buffer空间的 x y 为对应buffer的位置 z为深度。用于深度测试
	bool DrawTriangle3D(const char* ptri, const char* ptex, const char* puv, const char* pcl,
		bool zwrite, bool ztest)
	{
		// iz的递推获取 uv的递推获取
		// 全部用浮点数计算吧，干嘛用整数呢 复杂得不得了

		// 平底 三个点 上 左 右 z都相同 w是深度值
		float4 p1, p2, p3;
		// p1(x, y) p2(x, y) p3(x, y) 的三角形
		float dx1 = p2.x - p1.x;
		float dy1 = p2.y - p1.y;
		// 左边的点
		float4 pli;
		// 右边的点
		float4 pri;
		// 扫描线

	}

	// 窗口空间的点转到buffer空间
	lint2 WndToBuffer(const POINT& pos);

	lrecti WndToBuffer(const RECT& wndrec);

	// 绘制平底三角形
	void FillFB(int xt, int yt, int xb, int yb, float k, const largb& fill_cl);

	//绘制平顶三角形
	void FillFT(int xb, int yb, int xt, int yt, float k,  const largb& fill_cl);

	// 扫描线
	void ScanLine(int y, int x1, int x2, const largb& cl)
	{
		if (x1 > x2)
		{
			lswap(x1, x2);
		}
		for (int ix = x1; ix < x2; ix++)
		{
			backBuffer->SetXY(ix, y, cl);
		}
	}

	// buffer空间的三角形绘制过程


	// 深度值扫描线
	void ScanDepthZW(int y, int x1, int x2, float z1, float z2)
	{
		if (x1 > x2)
		{
			lswap(x1, x2);
		}
		float iz = z1;
		float dz = (z2 - z1) / (x2 - x1);
		for (int ix = x1; ix < x2; ix++)
		{
			//backBuffer->SetXY(ix, y, cl);
			//depthBuffer->SetXY(ix, y, iz);
			iz += dz;
		}
	}

	void DrawPoint(int x, int y, float z, largb cl)
	{
		bool bz = z < GetDepth(x, y);
		// 需要深度检测，就
		if ( m_stateZTest )
		{
			if (bz)
			{
				backBuffer->SetXY(x, y, cl);
			}
		}
		else
		{
			backBuffer->SetXY(x, y, cl);
		}

		if (m_stateZWrote)
		{

		}
	}

	// 绘制像素点
	//bool DrawPixel(std::vector<stPixelInfo>& pixels);

	// 获取深度值
	float GetDepth(int x, int y);

	bool m_stateZTest;
	bool m_stateZWrote;

private:
	//HDC hdcWnd;
	int bufferWidth;
	int bufferHeight;
	//HDC  hdcBitmap;
	HWND hRenderWnd;
	BITMAPINFO m_BmpInfo;

	void DrawLineBresenham(const lline2di& line, unsigned int cl, int linewWidth = 1);

	// 画线的点 y主导的方向 指定宽度宽度
	inline void _DrawPointX(float ixf, int iy, largb cl, int width)
	{

		if (width >0)
		{
			int ixmid = (int)roundf(ixf);

			// 宽度为1
			if (width == 1)
			{
				backBuffer->SetXY(ixmid, iy, cl);
				return;
			}

			if (width == 2)
			{
				//_DrawPointXEX(ixf, iy, cl);
				//return;
			}
			 
			// 由宽度获取上下两个点
			float ixup = ixf + (float)(width) / 2;
			//float ixdown = ixf - width - 1;
			int ixup_i = (int)floor(ixup);
			int ixdown_i = ixup_i - width + 1;

			// 决定画哪一个点
			// 0~0.5 模糊 0.5 ~0.9 不模糊  

			// 上下端点作模糊处理 如果是舍的点，就进行模糊
			float w = ixup - ixup_i;
		  
			// 画之间的点 1 2 3
			for (int iwidth = ixdown_i + 1; iwidth < ixup_i; iwidth++)
			{
				backBuffer->SetXY(iwidth, iy, cl);
			}
			// 画下端点
			backBuffer->SetXY(ixdown_i, iy, cl, (1.f - w));
			// 画上端点
			backBuffer->SetXY(ixup_i, iy, cl,  w);
		}
		
	}

	// 画线的点 x主导的方向 指定宽度宽度
	inline void _DrawPointY(int ix, float iyf, largb cl, float width)
	{
		if (width > 0)
		{
			int iymid = (int)roundf(iyf);
			// 宽度为1
			if (width == 1)
			{
				backBuffer->SetXY(ix, iymid, cl);
				return;
			}

			if (width == 2)
			{
				//return _DrawPointYEX(ix, iyf, cl);
			}

			// 由宽度获取上下两个点
			float iyup = iyf + (float)(width) / 2;
			int iyup_i = (int)floor(iyup);
			int iydown_i = (int)(iyup_i - width + 1);
			// 上下端点作模糊处理
			float w = (iyup - iyup_i);
			// 边缘的两点的处理：05.~所占的比例

			// 画之间的点 1 2 3
			for (int iwidth = iydown_i ; iwidth < iyup_i; iwidth++)
			{
				backBuffer->SetXY(ix, iwidth, cl);
			}
			// 画下端点 占得多，就画一半透明度
			backBuffer->SetXY(ix, iydown_i, cl, (1.f - w));
			// 画上端点
			backBuffer->SetXY(ix, iyup_i, cl, w);
		}
	}


	// 两个像素点抗锯齿的方式
	inline void _DrawPointYEX(int ix, float iyf, largb cl)
	{
		float y1 = ceilf(iyf);
		// 取下面的整数
		float y2 = floorf(iyf);
		// 离得越近权重越高
		float w = y1 - iyf;
		float w1 = (w < 0.5f ? 1.f : 0.5f);
		float w2 = (w < .5f ? .5f : 1.f);
		// 两个点的权重进行画线
		//int iy = (int)roundf(iyf);
		//backBuffer->SetXY(ix, iy, cl);
		backBuffer->SetXY(ix, (int)y1, cl, (1-w));
		backBuffer->SetXY(ix, (int)y2, cl, (w));
	}

	// 两个像素点抗锯齿的方式
	inline void _DrawPointXEX(float ixf, int iy, largb cl)
	{
		// 取上面的整数 
		float x1 = ceilf(ixf);
		// 取下面的整数
		float x2 = floorf(ixf);
		// 离得越近权重越高
		float w = x1 - ixf;
		//float w1 = (w < 0.5f ? 1.f : 0.5f);
		//float w2 = (w < .5f ? .5f : 1.f);
		// 两个点的权重进行画线
		//backBuffer->SetXY(ix, iy, cl);
		backBuffer->SetXY((int)x1, iy, cl, (1-w));
		backBuffer->SetXY((int)x2, iy, cl, w);
	}


	// 绘制平顶三角形

	// 把一个三角形分为平底和平顶三角形

};