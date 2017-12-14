#pragma once

#include <math.h>

#include "lvector2.h"
#include "lvector3.h"

template<typename tv>
class  lvector4
{
public:
	tv x, y, z, w;

	lvector4() :x((tv)0), y((tv)0), z((tv)0), w((tv)0)
	{
	}

	lvector4(const tv& _x, const tv& _y, const tv& _z, const tv& _w)
		:x(_x), y(_y), z(_z), w(_w)
	{
	}

	lvector4(const tv* p) :x(p[0]), y(p[1]), z(p[2]), w(p[3])
	{
	}

	lvector4(const lvector3<tv>& v, tv _w) :x(v.x), y(v.y), z(v.z), w(_w)
	{}

	lvector4(const lvector4& v) :x(v.x), y(v.y), z(v.z), w(v.w)
	{
	}

	lvector4& operator=(const lvector4& v)
	{
		x = v.x; y = v.y; z = v.z; w = v.w;
		return *this;
	}

	lvector4 operator + (const lvector4& v)const
	{
		return lvector4(x + v.x, y + v.y, z + v.z, w + v.w);
	}

	lvector4& operator += (const lvector4& v)
	{
		x += v.x;
		y += v.y;
		z += v.z;
		w += v.w;
		return *this;
	}

	lvector4 operator - (const lvector4& v)const
	{
		return lvector4(x - v.x, y - v.y, z - v.z, w - v.w);
	}

	lvector4& operator -= (const lvector4& v)
	{
		x -= v.x;
		y -= v.y;
		z -= v.z;
		w -= v.w;
		return *this;
	}

	lvector4 operator * (const lvector4& v)const
	{
		return lvector4(x * v.x, y * v.y, z * v.z, w * v.w);
	}

	lvector4& operator *= (const lvector4& v)
	{
		x *= v.x;
		y *= v.y;
		z *= v.z;
		w *= v.w;
		return *this;
	}

	lvector4 operator * (const tv& v)const
	{
		return lvector4(x * v, y * v, z * v, w * v);
	}

	lvector4& operator *= (const tv& v)
	{
		x *= v;
		y *= v;
		z *= v;
		w *= v;
		return *this;
	}

	lvector4 operator / (const lvector4& v)const
	{
		return lvector4(x / v.x, y / v.y, z / v.z, w / v.w);
	}

	lvector4& operator /= (const lvector4& v)
	{
		x /= v.x;
		y /= v.y;
		z /= v.z;
		w /= v.w;
		return *this;
	}

	lvector4 operator / (const tv& v)const
	{
		return lvector4(x / v, y / v, z / v, w / v);
	}

	lvector4& operator /= (const tv& v)
	{
		x /= v;
		y /= v;
		z /= v;
		w /= v;
		return *this;
	}

	bool operator==(const lvector4& v)const
	{
		return lequal(x, v.x) && lequal(y, v.y) && lequal(z, v.z) && lequal(w, v.w);
	}

	bool operator!=(const lvector4& v)const
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
		return lsqrt(x*x + y*y + z*z + w*w);
	}

	tv dot(const lvector4& v)const
	{
		return x*v.x + y*v.y + z*v.z + w*v.w;
	}

	// 归一化
	lvector4 setnor()
	{
		tv inv_len = (tv)1 / (this->len());
		this->operator*=(inv_len);
		return *this;
	}

	lvector4& setclamp(tv vmin, tv vmax)
	{
		x = lclamp(x, vmin, vmax);
		y = lclamp(y, vmin, vmax);
		z = lclamp(z, vmin, vmax);
		w = lclamp(w, vmin, vmax);
		return *this;
	}

	lvector4 getclamp(tv vmin, tv vmax)const
	{
		return lvector4(
			lclamp(x, vmin, vmax),
			lclamp(y, vmin, vmax),
			lclamp(z, vmin, vmax),
			lclamp(w, vmin, vmax) );
	}

	// 获取归一化的值，不改变对象
	lvector4 getnor()const
	{
		return *this / len();
	}

	tv distance(const lvector4& v)const
	{
		return (*this - v).len();
	}

	tv angle(const lvector4& v)const
	{
		return lacos(this->dot(v) / (this->len()*v.len()));
	}

	lvector4 lerp(const lvector4& vec, const tv& vlerp)const
	{
		return lvector4(llerp(x, vec.x, vlerp), llerp(y, vec.y, vlerp), llerp(z, vec.z, vlerp), llerp(w, vec.w, vlerp));
	}

	lvector4& lerpeq(const lvector4& vec, const tv& vlerp)
	{
		x = llerp(x, vec.x, vlerp);
		y = llerp(y, vec.y, vlerp);
		z = llerp(z, vec.z, vlerp);
		w = llerp(w, vec.w, vlerp);
		return *this;
	}

	lvector3<tv> xyz()const
	{
		return lvector3<tv>(x, y, z);
	}

	lvector3<tv> xyw()const
	{
		return lvector3<tv>(x, y, w);
	}

	lvector3<tv>& xyz()
	{
		return *(lvector3<tv>*)this;
	}
};