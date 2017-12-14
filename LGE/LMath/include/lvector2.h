#pragma once

#include "lmath_core.h"



template<typename tv>
class  lvector2
{
public:
	tv x, y;

	lvector2() :x((tv)0), y((tv)0) 
	{}

	lvector2(const tv& _x, const tv& _y) :x(_x), y(_y) 
	{}

	lvector2(const tv* p) :x(p[0]), y(p[1]) 
	{}

	lvector2(const lvector2& v) :x(v.x), y(v.y) 
	{}

	lvector2& operator=(const lvector2& v)
	{
		x = v.x; y = v.y;
		return *this;
	}

	lvector2 operator + (const lvector2& v)const
	{
		return lvector2(x + v.x, y + v.y);
	}

	lvector2& operator += (const lvector2& v)
	{
		x += v.x;
		y += v.y;
		return *this;
	}

	lvector2 operator - (const lvector2& v)const
	{
		return lvector2(x - v.x, y - v.y);
	}

	lvector2& operator -= (const lvector2& v)
	{
		x -= v.x;
		y -= v.y;
		return *this;
	}

	lvector2 operator * (const lvector2& v)const
	{
		return lvector2(x * v.x, y * v.y);
	}

	lvector2& operator *= (const lvector2& v)
	{
		x *= v.x;
		y *= v.y;
		return *this;
	}

	lvector2 operator * (const tv& v)const
	{
		return lvector2(x * v, y * v);
	}

	lvector2& operator *= (const tv& v)
	{
		x *= v;
		y *= v;
		return *this;
	}

	lvector2 operator / (const lvector2& v)const
	{
		return lvector2(x / v.x, y / v.y);
	}

	lvector2& operator /= (const lvector2& v)
	{
		x /= v.x;
		y /= v.y;
		return *this;
	}

	lvector2 operator / (const tv& v)const
	{
		return lvector2(x / v, y / v);
	}

	lvector2& operator /= (const tv& v)
	{
		x /= v;
		y /= v;
		return *this;
	}

	bool operator==(const lvector2& v)const
	{
		return lequal(x, v.x) && lequal(y, v.y)
	}

	bool operator!=(const lvector2& v)const
	{
		return !(*this == v);
	}

	tv& operator[](size_t i)
	{
		return (&x)[i];
	}

	const tv& operator[](size_t i)const
	{
		return (&x)[i];
	}

	tv len()const
	{
		return lsqrt(x*x + y*y);
	}

	tv dot(const lvector2& v)const
	{
		return x*v.x + y*v.y;
	}

	void nor()
	{
		this->operator/=(this->len());
	}

	lvector2 getnor()const
	{
		return *this / len();
	}

	tv distance(const lvector2& v)const
	{
		return (*this - v).len();
	}

	tv angle(const lvector2& v)const
	{
		return lacos(this->dot(v) / (this->len()*v.len()));
	}

	lvector2 lerp(const lvector2& vec, const tv& vlerp)const
	{
		return lvector2(llerp(x, vec.x, vlerp), llerp(y, vec.y, vlerp));
	}

	lvector2& lerpeq(const lvector2& vec, const tv& vlerp)
	{
		x = llerp(x, vec.x, vlerp);
		y = llerp(y, vec.y, vlerp)
		return *this;
	}

	// 二维向量cross 结果是一个标量 几何意义为两个向量的平行四边形的面积（带符号）
	// 符号为+表示顺时针 -为逆时针
	tv cross(const lvector2& v)
	{
		return x * v.y - y * v.x;
	}

	static lvector2 make_max(const lvector2& v1, const lvector2& v2)
	{
		return lvector2(lmax(v1.x, v2.x), lmax(v1.y, v2.y));
	}

	static lvector2 make_min(const lvector2& v1, const lvector2& v2)
	{
		return lvector2(lmin(v1.x, v2.x), lmin(v1.y, v2.y));
	}

};

