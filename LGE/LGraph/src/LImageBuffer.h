#pragma once

#include <vector>
#include "lcolor.h"
#include "lmath.hpp"


class LImageBuffer
{
public:
	LImageBuffer(int w, int h)
	{
		colorVector.resize(w * h);
		mw = w;
		mh = h;
	}

	std::vector<largb> colorVector;
	
	int Width()const
	{
		return mw;//colorVector.size() > 0 ? colorVector[0].size() : 0;
	}

	int Height()const
	{
		return mh;//colorVector.size();
	}

	int Size()const
	{
		return Width() * Height();
	}

	bool Clear(const largb& cl)
	{
		for ( auto& icl : colorVector)
		{
			icl = cl;
		}
		return true;
	}

	// 按行列设置
	void SetWH(size_t w, size_t h, const largb& cl, float blend = 1.f)
	{
		if ( lequal( blend, 1.f))
		{
			colorVector[Pos(w, h)] = cl;

		}
		else
		{
			colorVector[Pos(w, h)].blend(cl, blend);
		}
	}

	// 按x y坐标设置
	void SetXY(size_t x, size_t y, const largb& cl, float blend = 1.f)
	{
		this->SetWH(y, x, cl, blend);
	}

	// 按x y坐标设置
	void SetXY(const lint2& pos, const largb& cl, float blend = 1.f)
	{
		this->SetWH(pos.y, pos.x, cl, blend);
	}

	largb Get(size_t w, size_t h)const
	{
		return  colorVector[Pos(w, h)];
	}

	size_t Pos(size_t w, size_t h)const
	{
		return w * mw + h;
	}

	const largb* Data()const
	{
		return colorVector.data();
	}

private:

	int mw, mh;
};