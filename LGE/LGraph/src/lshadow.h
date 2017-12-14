#pragma once

//////////////////////////////////////////////////////////////////////////
// 阴影绘制 阴影图的实现方式

#include "lsoft_shader.h"

// 生成shadowmap的顶点程序
extern const char lvs_shadow_map[] = "lvs_shadow_map";
template <>
struct ltvs<lvs_shadow_map>
{
	lnode_trans* world_trans;
	lview_trans*  view_trans;
	lproj_trans*  proj_trans;

	lmatrix4f matWV;

	typedef stVPosNorUV t_in;
	typedef stVPosClUV t_out;

	void update()
	{
		matWV = world_trans->mat * view_trans->mat;
	}

	t_out process(const t_in& vin)
	{
		t_out vout;
		vout.pos = matWV.trans_point(vin.pos);
		return vout;
	}

};

// 生成shadow map的像素程序
extern const char lps_shadow_map[] = "lps_shadow_map";
template <>
struct ltps<lps_shadow_map>
{

	typedef stVPosClUV t_in;
	ltsampler< lsampler_point> smp;

	lcolor process(const t_in& pin)
	{
		return lcolor(pin.pos.z, pin.pos.z, pin.pos.z);
	}
};

// 顶点结构
struct stVSOutShadow
{
	float3 pos;
	float3 nor_in_view;
	float2 uv;
	float3 pos_lit; //
	float3 pos_in_view;
};

// 接受shadew的顶点程序
extern const char lvs_receive_shadow[] = "lvs_receive_shadow";
template<>
struct ltvs<lvs_receive_shadow>
{
	lnode_trans* world_trans;
	lview_trans*  view_trans;
	lproj_trans*  proj_trans;
	lmatrix4f matWV;

	lview_trans* view_lit;
	lproj_trans* proj_lit;
	lmatrix4f matlit_world_view;

	typedef stVPosNorUV t_in;
	typedef stVSOutShadow t_out;

	void update()
	{
		matWV = world_trans->mat * view_trans->mat;
		matlit_world_view = world_trans->mat * view_lit->mat;
	}

	t_out process(const t_in& vin)
	{
		t_out vout;
		vout.pos = matWV.trans_point(vin.pos);
		vout.nor_in_view = matWV.trans_vector(vin.nor);
		vout.uv = vin.uv;
		vout.pos_lit =  proj_lit->trans( matlit_world_view.trans_point(vin.pos) );
		vout.pos_in_view = vout.pos;
		return vout;
	}
};

// 接受shadow的像素程序 须逐像素光照
extern const char lps_receive_shadow[] = "lps_receive_shadow";
template<>
struct ltps<lps_receive_shadow>
{
	ltsampler<> smp;
	ldepth_buffer* shadow_map;
	ltlighter  lighter;
	typedef stVSOutShadow t_in;

	float shiness;

	lcolor process(const t_in& pin)
	{
		// 取得阴影图中的深度值
		int shadow_x = liround(pin.pos_lit.x);
		int shadow_y = liround(pin.pos_lit.y);
		float dp;
		if (shadow_x < 0 || shadow_x > shadow_map->w - 1 || shadow_y < 0 || shadow_y > shadow_map->h - 1)
		{
			dp = 0.f;
		}
		else
		{
			// 取五个点平均
			dp = shadow_map->get(shadow_x, shadow_y);
			//dp += shadow_map->get( max(0, shadow_x - 1), shadow_y);
			//dp += shadow_map->get( min(shadow_x + 1, shadow_map->w -1), shadow_y);
			//dp += shadow_map->get(shadow_x, max(shadow_y - 1, 0));
			//dp += shadow_map->get(shadow_x - 1, min(shadow_y + 1, shadow_map->h - 1));
			//dp /= 5.f;
		}
		// 当前深度值与阴影图中的比较
		lcolor cl_lit;
		if (pin.pos_lit.z > dp + 0.005f)
		{
			// 在阴影里 只取环境光
			//cl_lit = lcolor::black();//lighter.sclt->ambient * lighter.sclt->ambient_power;
			cl_lit = lighter.amb();
		}
		else
		{
			// 在阴影外 进行像素级光照
			cl_lit = lighter.amb_dif_spe(pin.pos_in_view, pin.nor_in_view.getnor(), float3(0.f, 0.f, 0.f), shiness);
		}

		// 读取阴影贴图
		// 顶点转换到光的view空间
		if (smp.ptex)
		{
			auto texcl = smp(pin.uv.x, pin.uv.y);
			return (cl_lit * texcl).setclamp(0.f, 1.f);
		}
		else
			return cl_lit;
	}
};