#pragma once

#include <math.h>
#include <stdlib.h>

#pragma warning(disable:4018)

typedef unsigned int uint32;
typedef unsigned int dword;
typedef unsigned char uchar;

const float L_PI = 3.141592654f;
const float L_2PI = 6.283185307f;
const float L_1DIVPI = 0.318309886f;
const float L_1DIV2PI = 0.159154943f;
const float L_PIDIV2 = 1.570796327f;
const float L_PIDIV4 = 0.785398163f;
const float L_PI_DIV_180 = 0.0174532925f;

// ֵ��ȵ��ж�
template<typename tv>
inline bool lequal(const tv& v1, const tv& v2)
{
	return v1 == v2;
}

// ���ڵ���
template<typename tv>
inline bool lgreeq(const tv& v1, const tv& v2)
{
	return lequal(v1, v2) || v1 > v2;
}

// С�ڵ���
template<typename tv>
inline bool llesseq(const tv& v1, const tv& v2)
{
	return lequal(v1, v2) || v1 < v2;
}

// float�ػ�
inline bool lequal(const float& v1, const float& v2)
{
	return fabs(v1 - v2) < 0.000001f;
}


// double�ػ�

// ����
template<typename tv>
inline tv lsqrt(const tv& v)
{
	return (tv)sqrt((double)v);
}

// ��sin
template<typename tv>
inline tv lsin(const tv& v)
{
	return (tv)sin((double)v);
}

// ��cos
template<typename tv>
inline tv lcos(const tv& v)
{
	return (tv)cos((double)v);
}

// ��tan
template<typename tv>
inline tv ltan(const tv& v)
{
	return (tv)tan((double)v);
}

// ��asin
template<typename tv>
inline tv lasin(const tv& v)
{
	return (tv)asin((double)v);
}

// ��acos
template<typename tv>
inline tv lacos(const tv& v)
{
	return (tv)acos((double)v);
}

// ��atan
template<typename tv>
inline tv latan(const tv& v)
{
	return (tv)atan((double)v);
}

// ��atan2
template<typename tv>
inline tv latan2(const tv& v1, const tv& v2)
{
	return (tv)atan2((double)v, (double)v2);
}

// ��ֵ
template<typename tv>
inline tv llerp(const tv& v1, const tv& v2, const tv& vlerp)
{
	return v2 + (v1 - v2) * vlerp;
}



// ��Ȩ�� ��lerp����Ĳ���
template<typename tv>
inline tv lweight(const tv& v1, const tv& v2, const tv& vtar)
{
	return  (vtar - v2) / (v1 - v2);
}

template<typename tv>
inline void lswap(tv& v1, tv& v2)
{
	tv temp = v1;
	v1 = v2;
	v2 = temp;
}

template <typename T>
inline T lmin(const T& v1, const T& v2)
{
	return v1 < v2 ? v1 : v2;
}

template <typename T>
inline T lmax(const T& v1, const T& v2)
{
	return v1 > v2 ? v1 : v2;
}

template <typename T>
inline T lclamp(T v, T _min, T _max)
{
	//return v > _max ? _max : (v < _min ? _min : v);
	return lmin(lmax(v, _min), _max);
}

// �Ƕ�ת����
template <typename T>
inline T degree2radian(const T& v)
{
	return v * L_PI_DIV_180;
}

// ����ת�Ƕ�
template <typename T>
inline T radian2degree(const T& v)
{
	return v * L_1DIVPI * 180.f;
}

// ȡ�������
inline int lrand(int i_min = 0, int i_max = 100)
{
	return rand() % (i_max - i_min) + i_min;
}

// ���������
inline float lrandf(float f_min = 0.f, float f_max = 1.f)
{
	return fmodf((float)rand() / 1.11111f, f_max - f_min) + f_min;
}

// ȡ��������
template <typename t>
inline t lceil(t v)
{
	return  (t)(int)v;
}

// ȡС������
template <typename t>
inline t ldecimal(t v)
{
	return v - lceil(v);
}

// ��������
inline int liround(float v)
{
	return v > 0 ? ((int)(v - 0.5f) + 1) : ((int)(v - 0.5));
}

inline float lfround(float v)
{
	return (float)liround(v);
}




