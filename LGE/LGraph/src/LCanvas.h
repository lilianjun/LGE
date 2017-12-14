#pragma once

#include "LImageBuffer.h"
#include "lmath.hpp"
#include <assert.h>
#include <windows.h>

#include "LTriDrawer.h"

// ���ߵĻ�ˢ
struct LBrush
{
	// ��ɫ
	largb color;
	int w, h;
	// ���ص�����λ��
	std::vector<lint2> pixels;
	// ���ص��Ȩ��
	std::vector<float> weights;

};

typedef std::vector< std::vector<float> > LBrushEx;


// 2D���ƽӿ�
class LCanvas
{
public:
	LImageBuffer* backBuffer;
	std::vector<unsigned int> frontBuffer;

	LCanvas():backBuffer(nullptr), bufferHeight(0), bufferWidth(0), hRenderWnd(nullptr)
	{
	}

	// ����
	bool Create(HWND hwnd, int w, int h);

	void Destroy()
	{
		if (backBuffer)
		{
			delete backBuffer;
			backBuffer = nullptr;
		}
	}

	// ��ת��ʾ������
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

	// �ü��ӿ������
	lline2di ClampLine(const lline2di& line)const;

	// �ü��ӿ���ľ���
	lrecti ClampRect(const lrecti& rect)
	{
		return lrecti(ClampPoint(rect.pmin), ClampPoint(rect.pmax));
	}

	// �ü��ӿ����������
	ltri2di ClampTriangle(const ltri2di& tri);

	// ���ƾ���
	bool DrawRect(const lrecti& rect, const largb& cl_fill, const largb& cl_line, int lineWidth = 1 );

	// �ڻ������ϻ��Ƶ� size����Ĵ�С ��������
	bool DrawPointWnd(const POINT& pos, const largb& cl, int size = 0)
	{
		return this->DrawPoint(WndToBuffer(pos), cl, size);
	}

	// �û�ˢ�ڴ����ϻ���
	bool DrawPointWnd(const POINT& pos, const largb& cl, const LBrushEx& brush)
	{
		return this->DrawPoint(WndToBuffer(pos), cl, brush);
	}

	// ����
	bool DrawPoint(const lint2& pos, const largb& cl, int size = 0)
	{
		return this->DrawRect(lrecti(pos.x - size, pos.y - size, pos.x + size, pos.y + size), cl, cl);
	}

	// �û�ˢ����
	bool DrawPoint(const lint2& pos, const largb& cl, const LBrushEx& brush);

	// ���ڿռ仭��
	bool DrawLineWnd(const POINT& p1, const POINT& p2, const largb& cl, int lineWidth = 1)
	{
		return DrawLine(lline2di(WndToBuffer(p1), WndToBuffer(p2)), cl, lineWidth);
	}

	// ���ڿռ��û�ˢ����
	bool DrawLineWnd(const POINT& p1, const POINT& p2, const largb& cl, const LBrushEx& brush)
	{
		return DrawLine(lline2di(WndToBuffer(p1), WndToBuffer(p2)), cl, brush);
	}

	// �û�ˢ����
	bool DrawLine(const lline2di& line, const largb& cl, const LBrushEx& brush);

	// ����
	bool DrawLine(const lline2di& line, const largb& cl, int linewWidth = 1);

	// ���ڿռ仭������
	bool DrawTriangleWnd(const POINT& p1, const POINT& p2, const POINT& p3, const largb& line_cl, const largb& fill_cl, int lineWidth = 1)
	{
		return DrawTriangle( ltri2di( WndToBuffer(p1), WndToBuffer(p2), WndToBuffer(p3))
		, line_cl,  fill_cl,lineWidth);
	}

	// ��������
	bool DrawTriangle(const ltri2di& triangle, const largb& line_cl, const largb& fill_cl, int lineWidth = 1);

	// ����ͼ������
	bool DrawTexturedTriangle(const ltri2di& triangle, const char* ptex, const char* puv);

	// ��3D������ �ѱ任��buffer�ռ�� x y Ϊ��Ӧbuffer��λ�� zΪ��ȡ�������Ȳ���
	bool DrawTriangle3D(const char* ptri, const char* ptex, const char* puv, const char* pcl,
		bool zwrite, bool ztest)
	{
		// iz�ĵ��ƻ�ȡ uv�ĵ��ƻ�ȡ
		// ȫ���ø���������ɣ������������� ���ӵò�����

		// ƽ�� ������ �� �� �� z����ͬ w�����ֵ
		float4 p1, p2, p3;
		// p1(x, y) p2(x, y) p3(x, y) ��������
		float dx1 = p2.x - p1.x;
		float dy1 = p2.y - p1.y;
		// ��ߵĵ�
		float4 pli;
		// �ұߵĵ�
		float4 pri;
		// ɨ����

	}

	// ���ڿռ�ĵ�ת��buffer�ռ�
	lint2 WndToBuffer(const POINT& pos);

	lrecti WndToBuffer(const RECT& wndrec);

	// ����ƽ��������
	void FillFB(int xt, int yt, int xb, int yb, float k, const largb& fill_cl);

	//����ƽ��������
	void FillFT(int xb, int yb, int xt, int yt, float k,  const largb& fill_cl);

	// ɨ����
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

	// buffer�ռ�������λ��ƹ���


	// ���ֵɨ����
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
		// ��Ҫ��ȼ�⣬��
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

	// �������ص�
	//bool DrawPixel(std::vector<stPixelInfo>& pixels);

	// ��ȡ���ֵ
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

	// ���ߵĵ� y�����ķ��� ָ����ȿ��
	inline void _DrawPointX(float ixf, int iy, largb cl, int width)
	{

		if (width >0)
		{
			int ixmid = (int)roundf(ixf);

			// ���Ϊ1
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
			 
			// �ɿ�Ȼ�ȡ����������
			float ixup = ixf + (float)(width) / 2;
			//float ixdown = ixf - width - 1;
			int ixup_i = (int)floor(ixup);
			int ixdown_i = ixup_i - width + 1;

			// ��������һ����
			// 0~0.5 ģ�� 0.5 ~0.9 ��ģ��  

			// ���¶˵���ģ������ �������ĵ㣬�ͽ���ģ��
			float w = ixup - ixup_i;
		  
			// ��֮��ĵ� 1 2 3
			for (int iwidth = ixdown_i + 1; iwidth < ixup_i; iwidth++)
			{
				backBuffer->SetXY(iwidth, iy, cl);
			}
			// ���¶˵�
			backBuffer->SetXY(ixdown_i, iy, cl, (1.f - w));
			// ���϶˵�
			backBuffer->SetXY(ixup_i, iy, cl,  w);
		}
		
	}

	// ���ߵĵ� x�����ķ��� ָ����ȿ��
	inline void _DrawPointY(int ix, float iyf, largb cl, float width)
	{
		if (width > 0)
		{
			int iymid = (int)roundf(iyf);
			// ���Ϊ1
			if (width == 1)
			{
				backBuffer->SetXY(ix, iymid, cl);
				return;
			}

			if (width == 2)
			{
				//return _DrawPointYEX(ix, iyf, cl);
			}

			// �ɿ�Ȼ�ȡ����������
			float iyup = iyf + (float)(width) / 2;
			int iyup_i = (int)floor(iyup);
			int iydown_i = (int)(iyup_i - width + 1);
			// ���¶˵���ģ������
			float w = (iyup - iyup_i);
			// ��Ե������Ĵ���05.~��ռ�ı���

			// ��֮��ĵ� 1 2 3
			for (int iwidth = iydown_i ; iwidth < iyup_i; iwidth++)
			{
				backBuffer->SetXY(ix, iwidth, cl);
			}
			// ���¶˵� ռ�ö࣬�ͻ�һ��͸����
			backBuffer->SetXY(ix, iydown_i, cl, (1.f - w));
			// ���϶˵�
			backBuffer->SetXY(ix, iyup_i, cl, w);
		}
	}


	// �������ص㿹��ݵķ�ʽ
	inline void _DrawPointYEX(int ix, float iyf, largb cl)
	{
		float y1 = ceilf(iyf);
		// ȡ���������
		float y2 = floorf(iyf);
		// ���Խ��Ȩ��Խ��
		float w = y1 - iyf;
		float w1 = (w < 0.5f ? 1.f : 0.5f);
		float w2 = (w < .5f ? .5f : 1.f);
		// �������Ȩ�ؽ��л���
		//int iy = (int)roundf(iyf);
		//backBuffer->SetXY(ix, iy, cl);
		backBuffer->SetXY(ix, (int)y1, cl, (1-w));
		backBuffer->SetXY(ix, (int)y2, cl, (w));
	}

	// �������ص㿹��ݵķ�ʽ
	inline void _DrawPointXEX(float ixf, int iy, largb cl)
	{
		// ȡ��������� 
		float x1 = ceilf(ixf);
		// ȡ���������
		float x2 = floorf(ixf);
		// ���Խ��Ȩ��Խ��
		float w = x1 - ixf;
		//float w1 = (w < 0.5f ? 1.f : 0.5f);
		//float w2 = (w < .5f ? .5f : 1.f);
		// �������Ȩ�ؽ��л���
		//backBuffer->SetXY(ix, iy, cl);
		backBuffer->SetXY((int)x1, iy, cl, (1-w));
		backBuffer->SetXY((int)x2, iy, cl, w);
	}


	// ����ƽ��������

	// ��һ�������η�Ϊƽ�׺�ƽ��������

};