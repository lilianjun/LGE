#pragma once

#include "lmath.hpp"
#include "ltexture.h"
#include "lsoft_light.h"

//////////////////////////////////////////////////////////////////////////
// 软件模拟的shader

struct stVPosNorUV
{
	float3 pos;
	float3 nor;
	float2 uv;
};

struct stVPosUV
{
	float3 pos;
	float2 uv;
};

struct stVPosClUV
{
	float3 pos;
	lcolor cl;
	float2 uv;
};

// 节点变换
struct lnode_trans
{
	float3 pos, rot, scl;
	// 获取矩阵
	lmatrix4f mat;

	void update()
	{
		mat.settrans(pos, rot, scl);
	}
};

// view变换
struct lview_trans 
{
	float3 pos, up, dir;
	lmatrix4f mat;
	void update()
	{
		mat.setview(pos, up, dir);
	}
};

// 投影变换
struct lproj_trans
{
	float aspect, fov, zn, zf;
	// 视口
	float w_vp, h_vp;
	// 获取变换矩阵
	lmatrix4f mat;

	void update()
	{
		mat.setproj(fov, aspect, zn, zf);
	}

	// 变换顶点
	float3 trans(const float3& v)
	{
		return float3(
			(mat.m11 * v.x / v.z + 1.f) * w_vp / 2,
			(mat.m22 * v.y / v.z + 1.f) * h_vp / 2,
			v.z  //(mat.m33 * v.z + mat.m43)
		);
		//return float3( mat.m11 * v.x, mat.m22 * v.y, mat.m33 * v.z + mat.m43);
		//Zfar / (Zfar - Znear) + Znear*Zfar / (Znear - Zfar), 0);
		// zfar( z - zn ) /(zfar - zn)
	}
};

// Orthogonal 正交投影变换
struct lorth_proj
{
	float w, h, zn, zf, w_vp, h_vp;
	lmatrix4f mat;

	void update()
	{
		mat.setorth(w, h, zn, zf);
	}

	float3 trans(const float3& v)
	{
		// (x * 2 * w / z + 1.f) / 2
		return float3(
			(mat.m11 * v.x / v.z + 1.f) * w_vp / 2,
			(mat.m22 * v.y / v.z + 1.f) * h_vp / 2,
			(mat.m33 * v.z + mat.m43) / v.z
		);
	}

};

// 顶点着色器模板
extern const char lvs_default[] = "lvs_defualt";
template <const char* id = lvs_default>
struct ltvs
{
	lnode_trans* world_trans;
	lview_trans*  view_trans;
	lproj_trans*  proj_trans;
	ltlighter  lighter;
	lmatrix4f matWV;
	float shiness;

	typedef stVPosNorUV t_in;
	typedef stVPosClUV t_out;

	inline void update()
	{
		matWV = world_trans->mat * view_trans->mat;
	}

	inline t_out process(const t_in& vin)
	{
		t_out vout;
		vout.pos = matWV.trans_point(vin.pos);
		vout.cl = lighter.amb_dif_spe(vout.pos, matWV.trans_vector(vin.nor).setnor(), float3(0, 0, 0), shiness);
		vout.uv = vin.uv;
		return vout;
	}
};

// 没有高光的顶点程序
extern const char lvs_diffuse[] = "lvs_diffuse";
template <>
struct ltvs<lvs_diffuse>
{
	lnode_trans* world_trans;
	lview_trans*  view_trans;
	lproj_trans*  proj_trans;
	ltlighter  lighter;
	lmatrix4f matWV;
	float shiness;

	typedef stVPosNorUV t_in;
	typedef stVPosClUV t_out;

	inline void update()
	{
		matWV = world_trans->mat * view_trans->mat;
	}

	inline t_out process(const t_in& vin)
	{
		t_out vout;
		vout.pos = matWV.trans_point(vin.pos);
		vout.cl = lighter.amb_dif(vout.pos, matWV.trans_vector(vin.nor).setnor());
		vout.uv = vin.uv;
		return vout;
	}
};

// fx technique pass的模拟
template <int id>
struct lteffect
{
	// techniqe 
	// pass


};

struct lcolor_buffer 
{
	lcolor_buffer():w(0), h(0)
	{ }

	lcolor_buffer(int _w, int _h)
		: w(_w), h(_h), pbuf(_w*_h, largb::black())
	{ }

	std::vector<largb> pbuf;
	int w, h;

	largb get(int x, int y)
	{
		 return pbuf[w * y + x];
	}
	void set(int x, int y, largb cl)
	{
		pbuf[w * y + x] = cl;
	}

	void clear(largb cl)
	{
		for (auto& icl : pbuf)
		{
			icl = cl;
		}
	}

	auto data()
	{
		return pbuf.data();
	}
};

struct ldepth_buffer
{

	ldepth_buffer() :w(0), h(0) {}
	ldepth_buffer(int _w, int _h)
		: w(_w), h(_h), pbuf(_w*_h, 1.f)
	{
	}
	std::vector<float> pbuf;
	int w, h;
	bool test(int x, int y, float z)
	{
		if (x > -1 && x < w && y > -1 && y < h)
		{
			return  !(z > pbuf[w * y + x]);
		}
		else
			return false;
	}
	void set(int x, int y, float z)
	{
		pbuf[w * y + x] = z;
	}
	float get(int x, int y)const
	{
		return pbuf[w * y + x];
	}
	void clear(float f)
	{
		for (auto& v : pbuf)
		{
			v = f;
		}
	}
	auto data()
	{
		return pbuf.data();
	}
};

extern const char lps_default[] = "lps_default";
template <const char* id_ps = lps_default>
struct ltps
{
	ltsampler<lsampler_point> smp;

	typedef stVPosClUV t_in;

	lcolor process(const t_in& pin)
	{
		if (smp.ptex)
		{
			auto texcl = smp(pin.uv.x, pin.uv.y);
			return (pin.cl * texcl * 2.f).setclamp(0.f, 1.f);
		}
		else
			return pin.cl;
	}
};

// 像素级光照
struct stVPosNorPosUV 
{
	float3 pos_proj;
	float3 nor_in_view;
	float3 pos_in_view;
	float2 uv;
};

extern const char lvs_pixel_lit[] = "lvs_pixel_lit";
template <>
struct ltvs<lvs_pixel_lit>
{
	lnode_trans* world_trans;
	lview_trans*  view_trans;
	lproj_trans*  proj_trans;
	lmatrix4f matWV;

	typedef stVPosNorUV t_in;
	typedef stVPosNorPosUV t_out;

	void update()
	{
		matWV = world_trans->mat * view_trans->mat;
	}

	t_out process(const t_in& vin)
	{
		stVPosNorPosUV vout;
		vout.pos_proj = matWV.trans_point(vin.pos);
		vout.nor_in_view = matWV.trans_vector(vin.nor).setnor();
		vout.pos_in_view = matWV.trans_point(vin.pos);
		vout.uv = vin.uv;
		return vout;
	}
};

extern const char lps_pixel_lit[] = "lps_pixel_lit";
template <>
struct ltps<lps_pixel_lit>
{
	using t_in = stVPosNorPosUV;

	ltlighter lighter;
	ltsampler<lsampler_point> smp;
	float shiness;

	lcolor process(const t_in& pin)
	{
		// 像素级光照
		lcolor cl_lit = lighter.amb_dif_spe(pin.pos_in_view, pin.nor_in_view.getnor(), float3(0.f, 0.f, 0.f), shiness);
		
		if (smp.ptex)
		{
			return  saturate(cl_lit * smp(pin.uv.x, pin.uv.y));
		}
		else
		{
			return cl_lit;
		}
		
	}

};