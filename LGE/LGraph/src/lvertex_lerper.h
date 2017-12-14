#pragma once

#include "lmath.hpp"


// 顶点插值器
template <size_t nfloat>
lfloatn<nfloat> lvertex_lerpX(const lfloatn<nfloat>& p1, const lfloatn<nfloat>& p2, float x)
{
	// 透视修正 1/z线性
	float z1 = p1[2]; float z2 = p2[2];
	lfloatn<nfloat> zp1 = p1 / z1;
	lfloatn<nfloat> zp2 = p2 / z2;

}

template <size_t nfloat>
lfloatn<nfloat> lvertex_lerpY(const lfloatn<nfloat>& p1, const lfloatn<nfloat>& p2)
{

}

template <size_t nfloat>
lfloatn<nfloat> lvertex_lerpZ(const lfloatn<nfloat>& p1, const lfloatn<nfloat>& p2)
{

}

// 顶点递推器
// 以x主导 属性透视矫正
template <size_t nfloat>
struct lvertex_steperX
{
	lfloatn<nfloat> p1, p2, dv, cur, pi;

	int ncount, ncur;
	float z1, z2;

	lvertex_steperX() {}
	// 以x主导
	void beg(const float* _p1, const float* _p2)
	{
		memcpy(p1.data(), _p1, sizeof(float)*nfloat);
		memcpy(p2.data(), _p2, sizeof(float)*nfloat);

		ncur = 0;
		ncount = (int)(abs(p2[0] - p1[0]));
		cur = p1;

		// x 保留 z取倒数 其他除以z
		z1 = p1[2]; z2 = p2[2];
		p1[2] = 1.f / z1; p2[2] = 1.f / z2;
		p1[1] *= p1[2]; p2[1] *= p2[2];
		for (int i = 3; i < nfloat; i++)
		{
			p1[i] *= p1[2];
			p2[i] *= p2[2];
		}

		float k = fabs(1.f / (p2[0] - p1[0]));
		dv = (p2 - p1) * k;
		pi = p1;
		
	}

	void next()
	{
		pi += dv;
		cur = pi;
		float iz = 1.f / cur[2];
		// x 保留 z 取倒数 其他，乘以z
		cur[2] = iz;
		cur[1] *= iz; p2[1] *= iz;
		for (int i = 3; i < nfloat; i++)
		{
			cur[i] *= iz;
		}
		ncur++;
	}

	bool end()
	{
		 return (ncur > ncount);
	}
};

// 顶点递推器
// 以y主导 属性透视矫正
template <size_t nfloat>
struct lvertex_steperY
{
	lfloatn<nfloat> p1, p2, dv, cur, pi;

	int ncount, ncur;

	lvertex_steperY() {}

	float z1, z2;

	// 
	void beg(const float* _p1, const float* _p2)
	{
		memcpy(p1.data(), _p1, sizeof(float)*nfloat);
		memcpy(p2.data(), _p2, sizeof(float)*nfloat);

		ncur = 0;
		ncount = (int)(abs(p2[1] - p1[1]));
		cur = p1;

		// z:取倒数 y：保留 其他：除以z
		z1 = p1[2]; z2 = p2[2];
		p1[2] = 1.f / z1; p2[2] = 1.f / z2;
		p1[0] *= p1[2]; p2[0] *= p2[2];
		for (int i = 3; i < nfloat; i++)
		{
			p1[i] *= p1[2];
			p2[i] *= p2[2];
		}
		
		// 求每次的递推量
		float k = fabs(1.f / (p2[1] - p1[1]));
		dv = (p2 - p1) * k;
		// 递推开始值
		pi = p1;
	}

	void next()
	{
		pi += dv;
		cur = pi;
		float iz = 1.f / cur[2];
		// y 保留 z 取倒数 其他，乘以z
		cur[2] = iz;
		cur[0] *= iz; p2[0] *= iz;
		for (int i = 3; i < nfloat; i++)
		{
			cur[i] *= iz;
		}

		ncur++;
	}

	bool end()
	{
		 return (ncur > ncount);
	}
};
