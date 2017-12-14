#pragma once

#include "lmath_core.h"
#include "lvector4.h"
#include "lshape3d.h"

template<typename tv>
class lmatrix4
{
public:
	tv	m11, m12, m13, m14,
		m21, m22, m23, m24,
		m31, m32, m33, m34,
		m41, m42, m43, m44;
	
	typedef lvector3<tv> tv3;
	typedef lvector4<tv> tv4;

	lmatrix4() :
		m11(1), m12(0), m13(0), m14(0),
		m21(0), m22(1), m23(0), m24(0),
		m31(0), m32(0), m33(1), m34(0),
		m41(0), m42(0), m43(0), m44(1)
	{
	}

	lmatrix4(
		tv _11, tv _12, tv _13, tv _14,
		tv _21, tv _22, tv _23, tv _24,
		tv _31, tv _32, tv _33, tv _34,
		tv _41, tv _42, tv _43, tv _44)
		:
		m11(_11), m12(_12), m13(_13), m14(_14),
		m21(_21), m22(_22), m23(_23), m24(_24),
		m31(_31), m32(_32), m33(_33), m34(_34),
		m41(_41), m42(_42), m43(_43), m44(_44)
	{ }






	// 变换向量
	tv3 trans_vector(const  tv3& p)const
	{
		return tv3(
			p.x * m11 + p.y * m21 + p.z * m31,
			p.x * m12 + p.y * m22 + p.z * m32,
			p.x * m13 + p.y * m23 + p.z * m33);
	}

	// 对顶点进行变换操作
	tv3 trans_point(const  tv3& p)const
	{
		return tv3(
			p.x * m11 + p.y * m21 + p.z * m31 + m41,
			p.x * m12 + p.y * m22 + p.z * m32 + m42,
			p.x * m13 + p.y * m23 + p.z * m33 + m43);
	}

	tv4 trans_point(const  tv4& p)const
	{
		return float4(
			p.x * m11 + p.y * m21 + p.z * m31 + p.w * m41,
			p.x * m12 + p.y * m22 + p.z * m32 + p.w * m42,
			p.x * m13 + p.y * m23 + p.z * m33 + p.w * m43,
			p.x * m14 + p.y * m24 + p.z * m34 + p.w * m44);
	}

	// 变换三角形
	void transform_triangle(ltri3d<tv>& triangle);

	// 设置变换
	void settrans(const tv3& pos, const tv3& rot, const tv3&scl)
	{
		*this = make_trans(pos, rot, scl);
	}

	static lmatrix4 make_rot(tv x, tv y, tv z)
	{
		//					1,		0,		0,			cy,			0,			-sy
		//					0,		cx,		sx,			0,			1,			0,
		//					0,		-sx,	cx			sy,			0,			cy)
		//
		//	cz,  sz, 0,		cz		sz*cx	sz*sx	cz*cy+sz*sx*sy	sz*cx		-cz*sy+sz*sx*cy
		//	-sz, cz, 0,		-sz		cz*cx	cz*sx	-sz*cy+cz*sx*sy cz*cx		sz*sy+cz*sx*cy
		//	0,    0, 1		0		-sx		cx			cx*sy		-sx			cx*cy
		//
		tv sx = (tv)lsin(x); tv sy = (tv)lsin(y); tv sz = (tv)lsin(z);
		tv cx = (tv)lcos(x); tv cy = (tv)lcos(y); tv cz = (tv)lcos(z);
		return lmatrix4(
			cz*cy + sz*sx*sy, sz*cx, -cz*sy + sz*sx*cy, (tv)0,
			-sz*cy + cz*sx*sy, cz*cx, sz*sy + cz*sx*cy, (tv)0,
			cx*sy, -sx, cx*cy, (tv)0,
			(tv)0, (tv)0, (tv)0, (tv)1);
	}

	static lmatrix4 make_rot(const tv3& rot)
	{
		return make_rot(rot.x, rot.y, rot.z);
	}

	static lmatrix4 make_trans(const tv3& pos, const tv3& rot, const tv3&scale)
	{
		lmatrix4 ret = lmatrix4::make_rot(rot);
		ret.m11 *= scale.x; ret.m12 *= scale.x; ret.m13 *= scale.x;
		ret.m21 *= scale.y; ret.m22 *= scale.y; ret.m23 *= scale.y;
		ret.m31 *= scale.z; ret.m32 *= scale.z; ret.m33 *= scale.z;

		ret.m41 = pos.x;
		ret.m42 = pos.y;
		ret.m43 = pos.z;
		return ret;
	}

	static lmatrix4 make_scale(const tv3& scl)
	{
		return lmatrix4(
			scl.x, 0, 0, 0,
			0, scl.y, 0, 0,
			0, 0, scl.z, 0,
			0, 0, 0, 1.f);
	}

	void setview(const tv3& pos, const tv3& up, const tv3& direction)
	{
		*this = make_view(pos, up, direction);
	}

	void setproj(float fov, float _aspect, float Znear, float Zfar)
	{
		*this = make_proj(fov, _aspect, Znear, Zfar);
	}

	void setorth(float w, float h, float zn, float zf)
	{
		*this = make_ortho(w, h, zn, zf);
	}

	// 设置view矩阵
	static lmatrix4 make_view(const tv3& pos, const tv3& up, const tv3& direction)
	{
		tv3 zaxis = direction.getnor();
		tv3 xaxis = up.getcross(zaxis).getnor();
		tv3 yaxis = zaxis.getcross( xaxis);
		return lmatrix4(xaxis.x, yaxis.x, zaxis.x, (tv)0,
			xaxis.y, yaxis.y, zaxis.y, (tv)0,
			xaxis.z, yaxis.z, zaxis.z, (tv)0,
			-xaxis.getdot( pos), -yaxis.getdot( pos), -zaxis.getdot( pos), (tv)1);
	}

	// 设置投影矩阵 fov:视野角度 弧度
	static lmatrix4 make_proj(float fov, float _aspect, float Znear, float Zfar)
	{
		float yS = 1.0f / tanf(fov * 0.5f);
		float xS = yS * _aspect;

		return lmatrix4(
			xS, 0, 0, 0,
			0, yS, 0, 0,
			0, 0, Zfar / (Zfar - Znear),1,
			0, 0, Znear*Zfar / (Znear - Zfar), 0);
	}

	// 正交投影
	inline lmatrix4 make_ortho(float w, float h, float zn, float zf)
	{
		//2 / w		0			0           0
		//0			2 / h		0           0
		//0			0			1/(zf - zn) 0
		//0			0		-zn/(zf - zn)	1

		return lmatrix4(
			2 / w, 0, 0, 0,
			0, 2 / h, 0, 0,
			0, 0, 1 / (zf - zn), 0,
			0, 0, -zn / (zf - zn), 1);
	}
};

template <typename tv>
inline lmatrix4<tv> operator * (const lmatrix4<tv>& m1, const lmatrix4<tv>& m2)
{
	// 左行乘右列 ret[i,j] = dot( m1.row[i], m1.col[i] )
	//								 2.11, 2.12, 2.13, 2.14
	//								 2.21, 2.22, 2.23, 2.24
	//								 2.31, 2.32, 2.33, 2.34
	//								 2.41, 2.42, 2.43, 2.44
	//	 1.11, 1.12, 1.13, 1.14		
	//	 1.21, 1.22, 1.23, 1.24
	//	 1.31, 1.32, 1.33, 1.34
	//	 1.41, 1.42, 1.43, 1.44
	// 4x16 = 64次乘法
	return lmatrix4<tv>(
		// row1
		m1.m11*m2.m11 + m1.m12*m2.m21 + m1.m13*m2.m31 + m1.m14*m2.m41,	//ret.m11 =
		m1.m11*m2.m12 + m1.m12*m2.m22 + m1.m13*m2.m32 + m1.m14*m2.m42,	//ret.m12 =
		m1.m11*m2.m13 + m1.m12*m2.m23 + m1.m13*m2.m33 + m1.m14*m2.m43,	//ret.m13 =
		m1.m11*m2.m14 + m1.m12*m2.m24 + m1.m13*m2.m34 + m1.m14*m2.m44,	//ret.m14 =
																		//row2														 	//
		m1.m21*m2.m11 + m1.m22*m2.m21 + m1.m23*m2.m31 + m1.m24*m2.m41,	//ret.m21 =
		m1.m21*m2.m12 + m1.m22*m2.m22 + m1.m23*m2.m32 + m1.m24*m2.m42,	//ret.m22 =
		m1.m21*m2.m13 + m1.m22*m2.m23 + m1.m23*m2.m33 + m1.m24*m2.m43,	//ret.m23 =
		m1.m21*m2.m14 + m1.m22*m2.m24 + m1.m23*m2.m34 + m1.m24*m2.m44,	//ret.m24 =
																		// row3														 	//
		m1.m31*m2.m11 + m1.m32*m2.m21 + m1.m33*m2.m31 + m1.m34*m2.m41,	//ret.m31 =
		m1.m31*m2.m12 + m1.m32*m2.m22 + m1.m33*m2.m32 + m1.m34*m2.m42,	//ret.m32 =
		m1.m31*m2.m13 + m1.m32*m2.m23 + m1.m33*m2.m33 + m1.m34*m2.m43,	//ret.m33 =
		m1.m31*m2.m14 + m1.m32*m2.m24 + m1.m33*m2.m34 + m1.m34*m2.m44,	//ret.m34 =
																		// row4														 	//
		m1.m41*m2.m11 + m1.m42*m2.m21 + m1.m43*m2.m31 + m1.m44*m2.m41,	//ret.m41 =
		m1.m41*m2.m12 + m1.m42*m2.m22 + m1.m43*m2.m32 + m1.m44*m2.m42,	//ret.m42 =
		m1.m41*m2.m13 + m1.m42*m2.m23 + m1.m43*m2.m33 + m1.m44*m2.m43,	//ret.m43 =
		m1.m41*m2.m14 + m1.m42*m2.m24 + m1.m43*m2.m34 + m1.m44*m2.m44	//ret.m44 =
	);
}