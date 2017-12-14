#pragma once

//////////////////////////////////////////////////////////////////////////
// 水面的渲染

#include "lsoft_shader.h"
#include "lmesh.h"
#include "lrender_triangle.h"
#include "LTimer.h"

extern const char lvs_water[] = "lvs_water";
template <>
struct ltvs<lvs_water>
{
	lnode_trans* world_trans;
	lview_trans*  view_trans;
	lproj_trans*  proj_trans;

	lmatrix4f matWV;

	typedef stVPosNorUV t_in;
	typedef stVPosUV t_out;

	void update()
	{
		matWV = world_trans->mat * view_trans->mat;
	}

	t_out process(const t_in& vin)
	{
		t_out vout;
		vout.pos = matWV.trans_point(vin.pos);
		vout.uv = vin.uv;
		return vout;
	}

};

// 水面绘制着色器
extern const char lps_water[] = "lps_water";
template <>
struct ltps<lps_water>
{
	typedef stVPosUV t_in;
	ltsampler<lsampler_point> nor_smp;
	ltsampler<lsampler_point> cl_smp;
	// 反射贴图
	ltsampler<lsampler_point> reflect_smp;

	lnode_trans* world_trans;
	lview_trans* view_trans;
	lmatrix4f matWV;

	ltlighter lighter;
	float shiness;
	lcolor water_color;

	// uv的移动
	float2 uv_move;
	// uv的缩放（重复）
	float uv_scale;

	void update()
	{
		matWV = world_trans->mat * view_trans->mat;
	}


	lcolor process(const t_in& pin)
	{
		// uv移动模拟水面流动
		float2 uv = (uv_move + pin.uv * uv_scale) ;
		// 采样法线
		float3 nor = nor_smp(uv.x, uv.y).f4.xyz();

		// 将法线转换到view空间
		nor = matWV.trans_vector(nor).setnor();

		// 简单的 没有反射贴图
		// 反射率 dot(V, Nor)
		float reflectivity;


		// 折射表现为透明度，水面最后绘制
		// 折射率 透明度
		float refractivity = .4f;

		// 颜色：环境光+漫反射光+高光
		//lcolor cl_lt = lighter.amb_dif_spe(pin.pos, nor, float3(0.f, 0.f, 0.f), shiness);
		lcolor cl_tex = cl_smp(uv.x, uv.y);
		lcolor cl_water = cl_tex;//*cl_lt;
		cl_water.A = refractivity;
		return cl_water;
	}

};

struct lwater
{
	lmesh<stVPosNorUV>* water_mesh;
	ltexture* water_normal;
	ltexture* water_cl;
	lnode_trans trans;
	
	float uv_scale;

	void create()
	{
	}

	void render(lttri_list_render<lvs_water, lps_water>& draw_water)
	{
		static CTimer timer;
		static float2 uv_move(0.f, 0.f);

		trans.update();
		// 
		draw_water.bblend = true;
		draw_water.getvs()->world_trans = &trans;
		draw_water.getps()->nor_smp.ptex = water_normal;
		draw_water.getps()->cl_smp.ptex = water_cl;
		draw_water.getps()->world_trans = &trans;

		float ft = (float)timer.Frame() *  0.01f;
		uv_move.y += ft;
		uv_move.y = fmod(uv_move.y, 1.f);
		draw_water.getps()->uv_move = uv_move;
		draw_water.getps()->uv_scale = 10.f;

		draw_water.getvs()->update();
		draw_water.getps()->update();
		draw_water.bback_cull = false;
		draw_water.render(water_mesh->verts.data(), water_mesh->verts.size(), water_mesh->tris.data(), water_mesh->tris.size());
	}

};