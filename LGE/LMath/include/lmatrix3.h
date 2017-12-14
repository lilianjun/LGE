#pragma once

#include "lvector3.h"

template<typename tv>
class lmatrix3
{
public:

	union
	{
		struct
		{
			tv	m11, m12, m13,
				m21, m22, m23,
				m31, m32, m33;
		};

		struct
		{
			lvector3<tv> row1, row2, row3;
		};
	};

	lmatrix3() :
		m11(1), m12(0), m13(0),
		m21(0), m22(1), m23(0),
		m31(0), m32(0), m33(1)
	{
	}

	lmatrix3(const lmatrix3& mat) :
		m11(mat.m11), m12(mat.m12), m13(mat.m13),
		m21(mat.m21), m22(mat.m22), m23(mat.m23),
		m31(mat.m31), m32(mat.m32), m33(mat.m33)
	{
	}

	lmatrix3(
		const tv& _11, const tv& _12, const tv& _13,
		const tv& _21, const tv& _22, const tv& _23,
		const tv& _31, const tv& _32, const tv& _33
	) :
		m11(_11), m12(_12), m13(_13),
		m21(_21), m22(_22), m23(_23),
		m31(_31), m32(_32), m33(_33)
	{
	}

	lmatrix3(const tv* pm) :
		m11(pm[0]), m12(pm[1]), m13(pm[2]),
		m21(pm[4]), m22(pm[5]), m23(pm[6]),
		m31(pm[8]), m32(pm[9]), m33(pm[10])
	{
	}

	lmatrix3 operator = (const lmatrix3& src)
	{
		memcpy(this, &src, sizeof(lmatrix3));
		return *this;
	}

	// 单位矩阵
	static lmatrix3 IDentity();

	static lmatrix3 idt;

	// 获取列
	lvector3<tv> col(int i)const
	{
		//return lvector3<tv>(data()[0+i], 
	}

	// 获取数据指针
	tv* data()
	{
		return &m11;;
	}

	const tv* data()const
	{
		return &m11;
	}

	// 返回行
	lvector3<tv>& operator[](size_t i)
	{
		return (&row1)[i];
	}

	const lvector3<tv>& operator[](size_t i)const
	{
		return (&row1)[i];
	}

	// 求行列式值
	tv det()const
	{
		return 
			m11 * (m22 * m33 - m23 * m32) +
			m12 * (m23 * m31 - m21 * m33) +
			m13 * (m21 * m32 - m22 * m31);
	}

	// 获取逆矩阵，不改变值
	lmatrix3 get_inverse()const
	{
		return this->transpose();
	}

	// 将该矩阵求逆
	void inverse()
	{
		this->transpose();
	}

	// 获取转置矩阵
	lmatrix3 get_transpose()const
	{
		lmatrix3(
			mat.m11, mat.m21, mat.m31,
			mat.m12, mat.m22, mat.m32,
			mat.m13, mat.m23, mat.m33);
	}

	// 将该矩阵转置
	void transpose()
	{
		lswap(m12, m21);
		lswap(m13, m31);
		lswap(m23, m32);
	}

	// 创建缩放矩阵
	static lmatrix3 make_scale(const tv& x, const tv& y, const tv& z)
	{
		return lmatrix3(
			x, 0, 0,
			0, y, 0,
			0, 0, z);
	}

	// 创建缩放矩阵
	static lmatrix3 make_scale(const lvector3<tv>& scl)
	{
		return make_scale(scl.x, scl.y, scl.z);
	}

	// 创建旋转矩阵
	static lmatrix3 make_rot_x(const tv& angle_radian)
	{
		float c = lcos(angle_radian);
		float s = lsin(angle_radian);
		return lmatrix3(
			1, 0,  0,
			0, c,  s,
			0, -s, c);
	}

	static lmatrix3 make_rot_y(const tv& angle_radian)
	{
		float c = lcos(angle_radian);
		float s = lsin(angle_radian);
		return lmatrix3(
			c, 0, -s,
			0, 1, 0,
			s, 0, c);
	}

	static lmatrix3 make_rot_z(const tv& angle_radian)
	{
		float c = lcos(angle_radian);
		float s = lsin(angle_radian);
		return lmatrix3(
			c,  s, 0,
			-s, c, 0,
			0,  0, 1);
	}

	static lmatrix3 make_rot_xyz(const tv& x, const tv& y, const tv& z)
	{
		//					cy		0	-sy        cz				sz				0
		//			  X		0		1	0      X   -sz				cz				0
		//					sy		0	cy		    0				0				1 

		//	1   0   0 		 cy		0	-sy			cy*cz			cy*sz			-sy
		//	0   cx  sx		 sx*sy	cx	sx*cy		sx*sy*cz-cx*sz sx*sy*sz+cx*cz  sx*cy
		//	0 - sx  cx		 cx*sy	-sx	cx*cy		cx*sy*cz+sx*sz cx*sy*sz-sx*cz -sx+cx*cy

		float sx = lsin(x); float sy = lsin(y); float sz = lsin(z);
		float cx = lcos(x); float cy = lcos(y); float cz = lcos(z);
		return lmatrix3(
			cy*cz, cy*sz, -sy,
			sx*sy*cz - cx*sz, sx*sy*sz + cx*cz, sx*cy,
			cx*sy*cz + sx*sz, cx*sy*sz - sx*cz, cx*cy
		);
	}

	static lmatrix3 make_rot_xyz(const lvector3<tv>& xyz)
	{
		return make_rot_xyz(xyz.x, xyz.y, xyz.z);
	}

	static lmatrix3 make_rot_axis_anlge(const lvector3<tv>& axis, const tv& angle)
	{
		float x = axis.x, y = axis.y, z = axis.z;
		float sa = lsin(angle), ca = lcos(angle);
		float xx = x * x, yy = y * y, zz = z * z;
		float xy = x * y, xz = x * z, yz = y * z;

		return lmatrix3(
			xx + ca * (1 - xx), xy - ca * xy + sa * z, xz - ca * xz - sa * y,
			xy - ca * xy - sa * z, yy + ca * (1 - yy), yz - ca * yz + sa * x,
			xz - ca * xz + sa * y, yz - ca * yz - sa * x, zz + ca * (1 - zz)
		);
	}
};