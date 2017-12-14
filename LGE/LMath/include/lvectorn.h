#pragma once
#include <array>

// nά���� ֱ��ʹ�� std::array

template <typename tv, size_t nv>
using lvectorn = std::array<tv, nv>;

template <size_t nv>
using lfloatn = lvectorn<float, nv>;

template <size_t nv>
using lintn = lvectorn<int, nv>;

template <size_t nv>
lfloatn<nv>&& lmake_floatn(const float* p)
{
	lfloatn<nv> fn;
	for (size_t i = 0; i <  nv; i++)
	{
		fn[i] = p[i];
	}
	return std::move(fn);
}

template <typename tv, size_t nv>
inline lvectorn<tv, nv> operator+(const lvectorn<tv, nv>& v1, const lvectorn<tv, nv>& v2)
{
	auto v3 = v1;
	for (size_t i = 0; i < nv; i++)
	{
		v3[i] += v2[i];
	}
	return v3;
}

template <typename tv, size_t nv>
inline lvectorn<tv, nv>& operator+=(lvectorn<tv, nv>& v1, const lvectorn<tv, nv>& v2)
{
	for (size_t i = 0; i < nv; i++)
	{
		v1[i] += v2[i];
	}
	return v1;
}

template <typename tv, size_t nv>
inline lvectorn<tv, nv> operator-(const lvectorn<tv, nv>& v1, const lvectorn<tv, nv>& v2)
{
	auto v3 = v1;
	for (size_t i = 0; i < nv; i++)
	{
		v3[i] -= v2[i];
	}
	return v3;
}

template <typename tv, size_t nv>
inline lvectorn<tv, nv>& operator-=(lvectorn<tv, nv>& v1, const lvectorn<tv, nv>& v2)
{
	for (size_t i = 0; i < nv; i++)
	{
		v1[i] -= v2[i];
	}
	return v1;
}

template <typename tv, size_t nv>
inline lvectorn<tv, nv> operator*(const lvectorn<tv, nv>& v1, const lvectorn<tv, nv>& v2)
{
	auto v3 = v1;
	for (size_t i = 0; i < nv; i++)
	{
		v3[i] *= v2[i];
	}
	return v3;
}

template <typename tv, size_t nv>
inline lvectorn<tv, nv>& operator*=(lvectorn<tv, nv>& v1, const lvectorn<tv, nv>& v2)
{
	for (size_t i = 0; i < nv; i++)
	{
		v1[i] *= v2[i];
	}
	return v1;
}

template <typename tv, size_t nv>
inline lvectorn<tv, nv> operator/(const lvectorn<tv, nv>& v1, const lvectorn<tv, nv>& v2)
{
	auto v3 = v1;
	for (size_t i = 0; i < nv; i++)
	{
		v3[i] /= v2[i];
	}
	return v3;
}

template <typename tv, size_t nv>
inline lvectorn<tv, nv>& operator/=(lvectorn<tv, nv>& v1, const lvectorn<tv, nv>& v2)
{
	for (size_t i = 0; i < nv; i++)
	{
		v1[i] /= v2[i];
	}
	return v1;
}

//////////////////////////////////////////////////////////////////////////
// ������ֵ�ļ���
template <typename tv, size_t nv>
inline lvectorn<tv, nv> operator+(const lvectorn<tv, nv>& v1, const tv& v2)
{
	auto v3 = v1;
	for (size_t i = 0; i < nv; i++)
	{
		v3[i] += v2;
	}
	return v3;
}

template <typename tv, size_t nv>
inline lvectorn<tv, nv>& operator+=(lvectorn<tv, nv>& v1, const tv& v2)
{
	for (size_t i = 0; i < nv; i++)
	{
		v1[i] += v2;
	}
	return v1;
}

template <typename tv, size_t nv>
inline lvectorn<tv, nv> operator-(const lvectorn<tv, nv>& v1, const tv& v2)
{
	auto v3 = v1;
	for (size_t i = 0; i < nv; i++)
	{
		v3[i] -= v2;
	}
	return v3;
}

template <typename tv, size_t nv>
inline lvectorn<tv, nv>& operator-=(lvectorn<tv, nv>& v1, const tv& v2)
{
	for (size_t i = 0; i < nv; i++)
	{
		v1[i] -= v2;
	}
	return v1;
}

template <typename tv, size_t nv>
inline lvectorn<tv, nv> operator*(const lvectorn<tv, nv>& v1, const tv& v2)
{
	auto v3 = v1;
	for (size_t i = 0; i < nv; i++)
	{
		v3[i] *= v2;
	}
	return v3;
}

template <typename tv, size_t nv>
inline lvectorn<tv, nv>& operator*=(lvectorn<tv, nv>& v1, const tv& v2)
{
	for (size_t i = 0; i < nv; i++)
	{
		v1[i] *= v2;
	}
	return v1;
}

template <typename tv, size_t nv>
inline lvectorn<tv, nv> operator/(const lvectorn<tv, nv>& v1, const tv& v2)
{
	auto v3 = v1;
	for (size_t i = 0; i < nv; i++)
	{
		v3[i] /= v2;
	}
	return v3;
}

template <typename tv, size_t nv>
inline lvectorn<tv, nv>& operator/=(lvectorn<tv, nv>& v1, const tv& v2)
{
	for (size_t i = 0; i < nv; i++)
	{
		v1[i] /= v2;
	}
	return v1;
}

// �������ֵ
template <typename tv, size_t nv>
inline lvectorn<tv, nv> llerp(const lvectorn<tv, nv>& p1, const lvectorn<tv, nv>& p2, tv w)
{
	return (p1 - p2) * w + p2;
}

// ������һ�� 
// ������p1 p2���������� pos:��֪���ڵ�ṹ�е�λ�� v��֪��ֵ
template <typename tv, size_t nv>
inline lvectorn<tv, nv> lpoint_on_line(const lvectorn<tv, nv>& p1, const lvectorn<tv, nv>& p2, size_t pos, tv v)
{
	auto dv = p2[pos] - p1[pos];
	// ��������0
	if ( lequal(dv, (tv)0) )
	{
		return p1;
	}
	tv w = (p2[pos] - v) / dv ;
	return llerp(p1, p2, w);
}

template<typename tv, size_t nv>
struct llinen
{
	typedef lvectorn<tv, nv> t_point;
	t_point p1, p2;

	// ��һ��ֵ��������ֵ
	inline t_point&& point(size_t pos, tv v)
	{
		float w = (p2[pos] - v) / (p2[pos] - p1[pos]);
		return lerp(w);
	}

	// ��ֵ����֮��ĵ�
	inline t_point&& lerp( float w)
	{
		//return llerp(_p1, _p2, w);
		return (p1 - p2) * w + p2;
	}

};


// ������ �Ƶ����ϵĵ�
template<typename tv, size_t nv>
struct llinen_steper
{
	typedef lvectorn<tv, nv> t_point;
	
	t_point p1, p2;

	t_point cur;
	
	llinen_steper() = default;

	llinen_steper(const t_point& _p1, const t_point& _p2)
		:p1(_p1), p2(_p2)
	{ }

	llinen_steper(const tv* _p1, const tv* _p2)
	{
		memcpy(p1.data(), _p1, sizeof(tv)*nv);
		memcpy(p2.data(), _p2, sizeof(tv)*nv);
	}

	inline void beg(size_t _pos, tv d = (tv)1)
	{
		cur = p1;
		npos = _pos;
		auto k = fabs( d / (p2[_pos] - p1[_pos]) );
		dv = (p2 - p1) * k;
		ncur = 0;
		ncount = (int)(abs(p2[_pos] - p1[_pos]) / d);
	}

	inline bool end()
	{
		// �����������������v 
		// ���ڸ����������Ǻܴ�������
		//return lequal(cur[npos], p2[npos]);
		return !(ncur < ncount);
	}

	inline void next()
	{
		cur += dv;
		ncur++;
	}

private:
	t_point dv;
	size_t npos;
	size_t ncount;
	size_t ncur;
};