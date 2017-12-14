#pragma once

#include <vector>

template <typename T>
class TSurface;

typedef TSurface<largb> LColorBuffer;
typedef TSurface<float> LDepthBuffer;

template <typename T>
class TSurface
{
public:
	std::vector<T> values;

	int m_w, m_h;

	void Create(int _w, int _h)
	{
		m_w = _w; m_h = _h;
		values.resize(GetSize());
	}

	T GetXY();

	T GetWH();

	void SetXY(int x, int y, const T& cl)
	{
		Set(PosXY(x, y), cl);
	}

	void SetWH(int w, int h, const T& cl)
	{
		values[PosWH(w, h)] = cl;
	}

	void Set(int pos, const T& cl)
	{
		values[pos] = cl;
	}

	// 按x y坐标获取高度
	int PosXY(int x, int y)
	{
		return m_w * y + x;
	}

	// 按宽、高获取位置
	int PosWH(int w, int h)
	{
		return h * m_w + w;
	}

	int GetW()const
	{
		return m_w;
	}

	int GetH()
	{
		return m_h;
	}

	int GetSize()
	{
		return m_w * m_h;
	}

	void Clear(const T& cl)
	{
		for (auto& p : values)
		{
			p = cl;
		}
	}

	const T* Data()const
	{
		return values.data();
	}

	 T* Data()
	{
		return values.data();
	}
};