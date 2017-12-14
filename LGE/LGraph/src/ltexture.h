#pragma once

#include <vector>
#include <fstream>
#include <assert.h>
#include "math.h"

//////////////////////////////////////////////////////////////////////////
struct ltexture 
{
	int w, h;
	std::vector<unsigned int> buffer;
	unsigned int get(int x, int y)const
	{
		return buffer[w * y + x];
	}
};

//////////////////////////////////////////////////////////////////////////
// bmp位图加载

#pragma pack(push, 2)
struct bmpheader
{
	short bfType;        //2Bytes，必须为"BM"，即0x424D 才是Windows位图文件
	unsigned int bfSize;         //4Bytes，整个BMP文件的大小
	short bfReserved1;  //2Bytes，保留，为0
	short bfReserved2;  //2Bytes，保留，为0
	unsigned int bfOffBits;     //4Bytes，文件起始位置到图像像素数据的字节偏移量
};

struct bmpinfo
{
	unsigned int  biSize;    //4Bytes，INFOHEADER结构体大小，存在其他版本I NFOHEADER，用作区分
	int   biWidth;    //4Bytes，图像宽度（以像素为单位）
	int   biHeight;    //4Bytes，图像高度，+：图像存储顺序为Bottom2Top，-：Top2Bottom
	short   biPlanes;    //2Bytes，图像数据平面，BMP存储RGB数据，因此总为1
	short   biBitCount;         //2Bytes，图像像素位数
	int  biCompression;     //4Bytes，0：不压缩，1：RLE8，2：RLE4
	int  biSizeImage;       //4Bytes，4字节对齐的图像数据大小
	int   biXPelsPerMeter;   //4 Bytes，用象素/米表示的水平分辨率
	int   biYPelsPerMeter;   //4 Bytes，用象素/米表示的垂直分辨率
	int  biClrUsed;          //4 Bytes，实际使用的调色板索引数，0：使用所有的调色板索引
	int biClrImportant;     //4 Bytes，重要的调色板索引数，0：所有的调色板索引都重要
};
#pragma pop()

bool lload_bmp(const char* pfilename, ltexture& tex)
{
	assert(sizeof(bmpheader) == 14);
	assert(sizeof(bmpinfo) == 40);
	std::ifstream inf(pfilename, std::ios::binary);
	if (inf)
	{
		bmpheader header;
		inf.read((char*)&header, sizeof(header));
		//if (header.bfType == 0x424D)
		{
			bmpinfo info;
			inf.read((char*)&info, sizeof(info));
			assert(info.biCompression == BI_RGB);
			
			if (info.biBitCount == 24 )
			{
				int npix = info.biWidth * info.biHeight;
				std::vector<char> data(3 * npix);
				inf.seekg(header.bfOffBits, std::ios::beg);
				inf.read(data.data(), 3 * npix);
				inf.close();
				tex.buffer.resize(npix);
				if (info.biHeight > 0)
				{
					for (int i = 0; i < npix;i++ )
					{
						tex.buffer[i] = largb(data[i * 3 + 2],
							data[i * 3 + 1],
							data[i * 3 + 0]
						);
					}
					tex.w = info.biWidth;
					tex.h = info.biHeight;
					return true;
				}
				else
				{
					assert(0);
				}
				//inf.seekg(0, std::ios::beg);
			}
			else if (info.biBitCount == 32)
			{
				inf.seekg(header.bfOffBits, std::ios::beg);
				if( info.biHeight > 0)
				{
					auto npix = info.biHeight * info.biWidth;
					tex.buffer.resize( npix );
					inf.read((char*)tex.buffer.data(), npix * sizeof(int));
					tex.w = info.biWidth;
					tex.h = info.biHeight;
					inf.close();
					return true;
				}
				else
				{
					assert(0);
				}
				
			}
		}
	}
	return false;
}

// 【最近点贴图采样器】
// 构造：采样算法 贴图
// 输入：uv坐标 
// 输出：颜色值

static const int  lsampler_point = 0;

template <int id_smp = lsampler_point>
struct ltsampler
{
	const ltexture* ptex;
	lcolor operator()(float u, float v)
	{
		int iw = liround((ptex->w - 1) * u);
		int ih = liround((ptex->h) *  (1.f - v) );
		iw = iw < 0 ? (-iw / (ptex->w) + 1) * ptex->w + iw : iw;
		ih = ih < 0 ? (-ih / (ptex->h) + 1) * ptex->h + ih : ih;
		iw = iw > (ptex->w-1) ? iw % (ptex->w) : iw;
		ih = ih > (ptex->h-1) ? ih % (ptex->h) : ih;
		return lcolor(ptex->get(iw, ih));
	}
};

static const int lsampler_liner = 1;

// 【线性贴图采样器】
template <>
struct ltsampler<lsampler_liner>
{
	const ltexture* ptex;
	lcolor operator()(float u, float v)
	{
		// 采样上下左右四个点
		// 贴图的大小
		int tw = (ptex->w - 1);
		int th = (ptex->h - 1);
		float fw = tw * u;
		float fh = th * (1.f - v);

		int lx = (int)fw;
		int rx = min(lx + 1, tw);
		int by = (int)fh;
		int ty = min(by + 1, th);

		// 计算权重 到实际点的距离
		// 左下点距离
		float2 mid(fw, fh);
		float lbw = mid.distance(float2((float)lx, (float)by));
		// 右下点距离
		float rbw = mid.distance(float2(rx, by));
		// 左上点距离
		float ltw = mid.distance(float2(lx, ty));
		// 右上点距离
		float rtw = mid.distance(float2(rx, ty));

		// 加权平均 
		float w4 = 1.f / (lbw + rbw + ltw + rtw);
		lbw *= w4; rbw *= w4; ltw *= w4; rtw *= w4;

		// 左下点
		lcolor lb(ptex->get(lx, by));
		// 右下点
		lcolor rb(ptex->get(rx, by));
		// 左上点
		lcolor lt(ptex->get(lx, ty));
		// 右上点
		lcolor rt(ptex->get(rx, ty));

		// 加权得结果
		return (lb * lbw + rb * rbw + lt * ltw + rt * rtw).setclamp(0.f, 1.f);
		//return ((lb + rb + lt + rt) * .25f).setclamp(0.f, 1.f);
	}
};

// 【四点平均采样】
static const int lsampler_average = 2;
template <>
struct ltsampler<lsampler_average>
{
	const ltexture* ptex;
	lcolor operator()(float u, float v)
	{
		// 采样上下左右四个点
		// 贴图的大小
		int tw = (ptex->w - 1);
		int th = (ptex->h - 1);
		float fw = tw * u;
		float fh = th * (1.f - v);

		int lx = (int)fw;
		int rx = min(lx + 1, tw);
		int by = (int)fh;
		int ty = min(by + 1, th);


		// 左下点
		lcolor lb(ptex->get(lx, by));
		// 右下点
		lcolor rb(ptex->get(rx, by));
		// 左上点
		lcolor lt(ptex->get(lx, ty));
		// 右上点
		lcolor rt(ptex->get(rx, ty));

		return ((lb + rb + lt + rt) * .25f).setclamp(0.f, 1.f);
	}
};

// 各向异性
struct lsampler_anisotropic
{
	const ltexture* ptex;
	largb operator()(float u, float v)
	{
		assert(0);
		return 0;
	}
};