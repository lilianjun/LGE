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
// bmpλͼ����

#pragma pack(push, 2)
struct bmpheader
{
	short bfType;        //2Bytes������Ϊ"BM"����0x424D ����Windowsλͼ�ļ�
	unsigned int bfSize;         //4Bytes������BMP�ļ��Ĵ�С
	short bfReserved1;  //2Bytes��������Ϊ0
	short bfReserved2;  //2Bytes��������Ϊ0
	unsigned int bfOffBits;     //4Bytes���ļ���ʼλ�õ�ͼ���������ݵ��ֽ�ƫ����
};

struct bmpinfo
{
	unsigned int  biSize;    //4Bytes��INFOHEADER�ṹ���С�����������汾I NFOHEADER����������
	int   biWidth;    //4Bytes��ͼ���ȣ�������Ϊ��λ��
	int   biHeight;    //4Bytes��ͼ��߶ȣ�+��ͼ��洢˳��ΪBottom2Top��-��Top2Bottom
	short   biPlanes;    //2Bytes��ͼ������ƽ�棬BMP�洢RGB���ݣ������Ϊ1
	short   biBitCount;         //2Bytes��ͼ������λ��
	int  biCompression;     //4Bytes��0����ѹ����1��RLE8��2��RLE4
	int  biSizeImage;       //4Bytes��4�ֽڶ����ͼ�����ݴ�С
	int   biXPelsPerMeter;   //4 Bytes��������/�ױ�ʾ��ˮƽ�ֱ���
	int   biYPelsPerMeter;   //4 Bytes��������/�ױ�ʾ�Ĵ�ֱ�ֱ���
	int  biClrUsed;          //4 Bytes��ʵ��ʹ�õĵ�ɫ����������0��ʹ�����еĵ�ɫ������
	int biClrImportant;     //4 Bytes����Ҫ�ĵ�ɫ����������0�����еĵ�ɫ����������Ҫ
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

// ���������ͼ��������
// ���죺�����㷨 ��ͼ
// ���룺uv���� 
// �������ɫֵ

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

// ��������ͼ��������
template <>
struct ltsampler<lsampler_liner>
{
	const ltexture* ptex;
	lcolor operator()(float u, float v)
	{
		// �������������ĸ���
		// ��ͼ�Ĵ�С
		int tw = (ptex->w - 1);
		int th = (ptex->h - 1);
		float fw = tw * u;
		float fh = th * (1.f - v);

		int lx = (int)fw;
		int rx = min(lx + 1, tw);
		int by = (int)fh;
		int ty = min(by + 1, th);

		// ����Ȩ�� ��ʵ�ʵ�ľ���
		// ���µ����
		float2 mid(fw, fh);
		float lbw = mid.distance(float2((float)lx, (float)by));
		// ���µ����
		float rbw = mid.distance(float2(rx, by));
		// ���ϵ����
		float ltw = mid.distance(float2(lx, ty));
		// ���ϵ����
		float rtw = mid.distance(float2(rx, ty));

		// ��Ȩƽ�� 
		float w4 = 1.f / (lbw + rbw + ltw + rtw);
		lbw *= w4; rbw *= w4; ltw *= w4; rtw *= w4;

		// ���µ�
		lcolor lb(ptex->get(lx, by));
		// ���µ�
		lcolor rb(ptex->get(rx, by));
		// ���ϵ�
		lcolor lt(ptex->get(lx, ty));
		// ���ϵ�
		lcolor rt(ptex->get(rx, ty));

		// ��Ȩ�ý��
		return (lb * lbw + rb * rbw + lt * ltw + rt * rtw).setclamp(0.f, 1.f);
		//return ((lb + rb + lt + rt) * .25f).setclamp(0.f, 1.f);
	}
};

// ���ĵ�ƽ��������
static const int lsampler_average = 2;
template <>
struct ltsampler<lsampler_average>
{
	const ltexture* ptex;
	lcolor operator()(float u, float v)
	{
		// �������������ĸ���
		// ��ͼ�Ĵ�С
		int tw = (ptex->w - 1);
		int th = (ptex->h - 1);
		float fw = tw * u;
		float fh = th * (1.f - v);

		int lx = (int)fw;
		int rx = min(lx + 1, tw);
		int by = (int)fh;
		int ty = min(by + 1, th);


		// ���µ�
		lcolor lb(ptex->get(lx, by));
		// ���µ�
		lcolor rb(ptex->get(rx, by));
		// ���ϵ�
		lcolor lt(ptex->get(lx, ty));
		// ���ϵ�
		lcolor rt(ptex->get(rx, ty));

		return ((lb + rb + lt + rt) * .25f).setclamp(0.f, 1.f);
	}
};

// ��������
struct lsampler_anisotropic
{
	const ltexture* ptex;
	largb operator()(float u, float v)
	{
		assert(0);
		return 0;
	}
};