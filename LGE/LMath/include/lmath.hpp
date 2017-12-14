#pragma once

#include "lvector2.h"
#include "lvector3.h"
#include "lvector4.h"
#include "lmatrix3.h"
#include "lshape2d.h"
#include "lshape3d.h"
#include "lmatrix4.h"
#include "lcolor.h"
#include "lvectorn.h"

typedef lvector2<float> float2;
typedef lvector3<float> float3;
typedef lvector4<float> float4;
typedef lmatrix3<float> matrix3;
typedef lvector2<int> lint2;
typedef lvector3<int> lint3;
typedef lvector3<uint32> luint3;
typedef lrect<int> lrecti;
typedef lrect<float> lrectf;
typedef lline2d<int> lline2di;
typedef lline2d<float> lline2df;
typedef ltri2d<int> ltri2di;
typedef ltri3d<float> ltri3df;
typedef lmatrix4<float> lmatrix4f;
typedef lfrustum<float> lfrustumf;
typedef lcircle2d<int> lcircle2di;
typedef lbox<float> lboxf;

inline float saturate(float v)
{
	return lclamp(v, 0.f, 1.f);
}

inline lcolor saturate(const lcolor& cl)
{
	return cl.getclamp(0.f, 1.f);
}

inline float dot(const float3& v1, const float3& v2)
{
	return float3::dot(v1, v2);
}

inline float3 cross(const float3& v1, const float3& v2)
{
	return float3::cross(v1, v2);
}

inline float length(const float3& v)
{
	return v.len();
}

inline float lengthsqr(const float3& v)
{
	return dot(v, v);
}

inline float3 normalize(const float3& v)
{
	return v.getnor();
}

// v = i - 2 * n * dot(i•n) 
inline float3 reflect(const float3& I, const float3& N)
{
	return I - 2 * dot(I, N) * N;
}
