#pragma once

#include <math.h>
#include "lvector2.h"


template<typename tv>
class  lvector3
{
public:
	tv x, y, z;

	lvector3() :x((tv)0), y((tv)0), z((tv)0)
	{
	}

	lvector3(const tv& _x, const tv& _y, const tv& _z) 
		:x(_x), y(_y), z(_z)
	{
	}

	lvector3(const tv* p) :x(p[0]), y(p[1]), z(p[2])
	{
	}

	lvector3(const lvector3& v) :x(v.x), y(v.y), z(v.z)
	{
	}

	lvector3& operator=(const lvector3& v)
	{
		x = v.x; y = v.y; z = v.z;
		return *this;
	}

	lvector3 operator + (const lvector3& v)const
	{
		return lvector3(x + v.x, y + v.y, z+v.z);
	}

	lvector3& operator += (const lvector3& v)
	{
		x += v.x;
		y += v.y;
		z += v.z;
		return *this;
	}

	lvector3 operator - (const lvector3& v)const
	{
		return lvector3(x - v.x, y - v.y, z-v.z);
	}

	lvector3& operator -= (const lvector3& v)
	{
		x -= v.x;
		y -= v.y;
		z -= v.z;
		return *this;
	}

	lvector3 operator * (const lvector3& v)const
	{
		return lvector3(x * v.x, y * v.y, z * v.z);
	}

	lvector3& operator *= (const lvector3& v)
	{
		x *= v.x;
		y *= v.y;
		z *= v.z;
		return *this;
	}

	lvector3 operator * (const tv& v)const
	{
		return lvector3(x * v, y * v, z * v);
	}

	lvector3& operator *= (const tv& v)
	{
		x *= v;
		y *= v;
		z *= v;
		return *this;
	}

	lvector3 operator / (const lvector3& v)const
	{
		return lvector3(x / v.x, y / v.y, z/v.z);
	}

	lvector3& operator /= (const lvector3& v)
	{
		x /= v.x;
		y /= v.y;
		z /= v.z;
		return *this;
	}

	lvector3 operator / (const tv& v)const
	{
		return lvector3(x / v, y / v, z / v);
	}

	lvector3& operator /= (const tv& v)
	{
		x /= v;
		y /= v;
		z /= v;
		return *this;
	}

	bool operator==(const lvector3& v)const
	{
		return lequal(x, v.x) && lequal(y, v.y) && lequal(z, v.z);
	}

	bool operator!=(const lvector3& v)const
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
		return lsqrt(x*x + y*y + z*z);
	}

	tv getdot(const lvector3& v)const
	{
		return x*v.x + y*v.y + z*v.z;
	}

	// 归一化
	lvector3& setnor()
	{
		tv inv_len = (tv)1 / (this->len());
		this->operator*=(inv_len);
		return *this;
	}

	// 获取归一化的值，不改变对象
	lvector3 getnor()const
	{
		return *this / len();
	}

	tv distance(const lvector3& v)const
	{
		return (*this - v).len();
	}

	tv angle(const lvector3& v)const
	{
		return lacos(this->dot(v) / (this->len()*v.len()));
	}

	lvector3 getcross(const lvector3& v2)const
	{
		return lvector3(y*v2.z - z*v2.y, z*v2.x - x*v2.z, x*v2.y - y*v2.x);
	}

	lvector3& crosseq(const lvector3& v2)
	{
		*this = this->cross(v2);
		return *this;
	}

	lvector3 lerp(const lvector3& vec, const tv& vlerp)const
	{
		return lvector3(llerp(x, vec.x, vlerp), llerp(y, vec.y, vlerp), llerp(z, vec.z, vlerp));
	}

	lvector3& lerpeq(const lvector3& vec, const tv& vlerp)
	{
		x = llerp(x, vec.x, vlerp);
		y = llerp(y, vec.y, vlerp);
		z = llerp(z, vec.z, vlerp);
		return *this;
	}

	lvector3 getclamp(tv vmin, tv vmax)const
	{
		return lvector3(
			lclamp(x, vmin, vmax),
			lclamp(y, vmin, vmax),
			lclamp(z, vmin, vmax));
	}
	// 限定值的范围
	lvector3& setclamp(tv vmin, tv vmax)
	{
		x = lclamp(x, vmin, vmax);
		y = lclamp(y, vmin, vmax);
		z = lclamp(z, vmin, vmax);
		return *this;
	}

	lvector2<tv> xy()const
	{
		return lvector2<tv>(x, y);
	}

	lvector2<tv>& xy()
	{
		return *(lvector2<tv>*)this;
	}

	lvector2<tv> yz()const
	{
		return lvector2<tv>(y, z);
	}

	lvector2<tv>& yz()
	{
		return *(lvector2<tv>*)&this->y;
	}

	lvector2<tv> xz()const
	{
		return lvector2<tv>(x, z);
	}

	static tv dot(const lvector3& v1, const lvector3& v2)
	{
		return v1.getdot(v2);
	}

	static lvector3 cross(const lvector3& v1, const lvector3& v2)
	{
		return v1.getcross(v2);
	}

	// 计算反射
	// R反射光的求解：R = 2 * dot(N , L) * N - L 【推导：L+R = 2 * dot(N, L) * N  (平行四边形 点乘的意义)
	//		2 * dot(N,L) = dot(L, R)
	static lvector3 reflect(const lvector3 & L, const lvector3 & N)
	{
		return N * dot(N, L) * (tv)2 - L;
	}


// 	static lvector2<tv> make_max(const lvector2<tv>& v1, const lvector2<tv>& v2)
// 	{
// 		return lvector2<tv>(lmax(v1.x, v2.x), lmax(v1.y, v2.y));
// 	}
// 
// 	static lvector2 make_min(const lvector2& v1, const lvector2& v2)
// 	{
// 		return lvector2(lmin(v1.x, v2.x), lmin(v1.y, v2.y));
// 	}
};

template <typename tv>
lvector3<tv> operator*(tv v, const lvector3<tv>& v3)
{
	return v3 * v;
}

template <typename tv>
lvector3<tv> operator-(const lvector3<tv>& v3)
{
	return lvector3<tv>(-v3.x, -v3.y, -v3.z);
}