#pragma once

#include "lmath_core.h"
#include "lvector3.h"

class lcolor;

struct largb
{
	union
	{

		struct  
		{
			uchar b, g, r, a;
		};
		dword value;
	};
	

	largb():value(0)
	{ }

	explicit largb(uchar _r, uchar _g, uchar _b)
		: r(_r), g(_g), b(_b), a(255)
	{ }

	largb(uchar _a, uchar _r, uchar _g, uchar _b)
		: r(_r), g(_g), b(_b), a(_a)
	{
	}

	largb(dword v) :value(v)
	{

	}

	largb& operator *= (float f)
	{
		r *= f; g *= f; b *= f;
		return *this;
	}

	largb operator + (const largb& v)const
	{
		return largb(r + v.r, g + v.g, b + v.b);
	}

	largb& operator += (const largb& v)
	{
		r += v.r; g += v.g; b += v.b;
		return *this;
	}

	// 颜色的混合 启用目标的alpha通道
	void blend(const largb& cl)
	{
		this->blend(cl, _toflt(cl.a));
	}

	void blend(const largb& cl, float f)
	{
		*this *= (1.0f - f);
		*this += (cl * f);
	}

	operator dword()const
	{
		 return value;
	}

	 static largb red(float f = 1.f)
	 {
		 return create(f, 0.f, 0.f);
	 }

	 static largb green(float f = 1.f)
	 {
		 return create(0.f, f, 0.f);
	 }
	 
	 static largb blue(float f = 1.f)
	 {
		 return create(0.f, 0.f, f);
	 }

	 static largb white()
	 {
		 return largb((uchar)255, (uchar)255, (uchar)255);
	 }

	 static largb gray(float f = 0.5f)
	 {
		 return create(f, f, f);
	 }


	 static largb black()
	 {
		 return largb((uchar)0, (uchar)0, (uchar)0);
	 }

	 static float _toflt(uchar c)
	 {
		 static const float _v = 1.0f / 255.f;
		 return _v * c;
	 }

	 inline largb operator * ( float f)const
	 {
		// return largb((uchar)(lclamp(r * f, 0.f, 255.f), (uchar)lclamp(g * f, 0.f, 255.f), (uchar)lclamp(b * f, 0.f, 255.f)));
		// return largb::create(fr() * f, fg() * f, fb() * f);
		 f = lclamp(f, 0.f, 1.f);
		 return largb((uchar)roundf(r*f), (uchar)roundf(g*f), (uchar)roundf(b*f));
	 }

	 static largb create(const lvector3<float>& f3)
	 {
		 return create(f3.x, f3.y, f3.z);
	 }

	 static largb create(float _r , float _g, float _b)
	 {
		 return largb(
			 (uchar)(_r * 255.f),
			 (uchar)(_g * 255.f),
			 (uchar)(_b * 255.f)
		 );
	 }

	 // 颜色的插值
	 static largb lerp(const largb& cl1, const largb& cl2, float w)
	 {
		 return cl1 * w + cl2 * (1.f - w);
	 }

	 // 三色插值
	 static largb lerp(const largb& cl1, float w1, const largb& cl2, float w2, const largb& cl3);

	 float fr()const
	 {
		 return r / 255.f;
	 }

	 float fg()const
	 {
		 return g / 255.f;
	 }

	 float fb()const
	 {
		 return b / 255.f;
	 }

	 float fa()const
	 {
		 return a / 255.f;
	 }

	 lvector3<float> frgb()const
	 {
		 return lvector3<float>(fr(), fg(), fb());
	 }

	
};


inline largb operator * ( float f, const largb& cl)
{
	//return largb((uchar)(lclamp(cl.r * f, 0.f, 255.f), (uchar)lclamp(cl.g * f, 0.f, 255.f), (uchar)lclamp(cl.b * f, 0.f, 255.f)));
	//return largb((uchar)(cl.r * f), (uchar)(cl.g * f), (uchar)(cl.b * f));
	return largb::create(cl.fr() * f, cl.fg() * f, cl.fb() * f);
}

#include "lvector4.h"
class lcolor
{
public:
	union 
	{
		struct  
		{
			float R, G, B, A;
		};
		lvector4<float> f4;
	};

	

	lcolor():R(0.f), G(0.f), B(0.f), A(1.f)
	{ }

	lcolor(float _r, float _g, float _b, float _a = 1.f)
		: R(_r), G(_g), B(_b), A(_a)
	{
	}

	lcolor(const lcolor& v):f4(v.f4)
	{ }

	lcolor(const largb& v) :R(v.fr()), G(v.fg()), B(v.fb()), A(v.fa())
	{

	}

	lcolor& operator=(const lcolor& v)
	{
		f4 = v.f4;
		return *this;
	}

	lcolor operator + (const lcolor& v)const
	{
		return *(lcolor*)&f4.operator+(v.f4);
	}

	lcolor& operator += (const lcolor& v)
	{
		f4.operator+=(v.f4);
		return *this;
	}

	lcolor operator - (const lcolor& v)const
	{
		return *(lcolor*)&f4.operator-(v.f4);
	}

	lcolor& operator -= (const lcolor& v)
	{
		f4.operator-=(v.f4);
		return *this;
	}

	lcolor operator * (const lcolor& v)const
	{
		return *(lcolor*)&f4.operator*(v.f4);
	}

	lcolor& operator *= (const lcolor& v)
	{
		f4.operator*=(v.f4);
		return *this;
	}

	lcolor operator / (const lcolor& v)const
	{
		return *(lcolor*)&f4.operator/(v.f4);
	}

	lcolor& operator /= (const lcolor& v)
	{
		f4.operator/=(v.f4);
		return *this;
	}

	lcolor operator * (const float& v)const
	{
		return *(lcolor*)&(f4.operator*(v));
	}

	lcolor& operator *= (const float& v)
	{
		f4.operator*=(v);
		return *this;
	}

	lcolor& setclamp(float vmin, float vmax)
	{
		f4.setclamp(vmin, vmax);
		return *this;
	}

	lcolor getclamp(float vmin, float vmax)const
	{
		return *(lcolor*)&f4.getclamp(vmin, vmax);
	}

	unsigned char r()const
	{
		return  (unsigned char)(R * 255);
	}

	unsigned char g()const
	{
		return  (unsigned char)(G * 255);
	}

	unsigned char b()const
	{
		return  (unsigned char)(B * 255);
	}

	unsigned char a()const
	{
		return  (unsigned char)(A * 255);
	}

	unsigned int get_bgra()const
	{
		return b() << 24 | g() << 16 | r() << 8 | a();
	}

	unsigned int get_argb()const
	{
		return a() << 24 | r() << 16 | g() << 8 | b();
	}

	static lcolor create(largb cl)
	{
		return lcolor(cl.fr(), cl.fg(), cl.fb(), cl.fa());
	}

	//lcolor& operator =(const lcolor& v) = default;
	static inline lcolor red(float v = 1.f)
	{
		return lcolor(v, 0.f, 0.f, 1.f);
	}

	static inline lcolor white()
	{
		return lcolor(1.0f, 1.f, 1.f, 1.f);
	}

	static inline lcolor black()
	{
		return lcolor(0.0f, 0.f, 0.f, 1.f);
	}

	static inline lcolor blue(float v = 1.f)
	{
		return lcolor(0.0f, 0.f, v, 1.f);
	}

	static inline lcolor green(float v = 1.f)
	{
		return lcolor(0.0f, v, 0.f, 1.f);
	}

	static inline lcolor gray(float v = .5f)
	{
		return lcolor(v, v, v, 1.f);
	}
};

inline lcolor operator*(float f, const lcolor& cl)
{
	return cl * f;
}