#pragma once

// 地形
#include "lsoft_shader.h"
#include "lmesh.h"

// 地形顶点程序

// 地形的ps
extern const char lps_terrain[] = "lps_terrain";
template <>
struct ltps<lps_terrain>
{
	ltsampler<lsampler_point> smp1, smp2, smp3, smp4, smp_alpha;

	float uv_scale;

	// 地形接受阴影

	typedef stVPosClUV t_in;

	inline lcolor process(const t_in& pin)
	{
		float2 uv = pin.uv * uv_scale;

		lcolor cl1 = smp1(uv.x, uv.y);
		lcolor cl2 = smp2(uv.x, uv.y);
		lcolor cl3 = smp3(uv.x, uv.y);
		lcolor cl4 = smp4(uv.x, uv.y);
		//lcolor cl5 = smp5(uv.x, uv.y);

		lcolor cl_a = smp_alpha(pin.uv.x, pin.uv.y);
		float w4 = 1.f - cl_a.R - cl_a.G - cl_a.B;

		lcolor cl_blend;
		cl_blend.f4.xyz() = cl1.f4.xyz() * cl_a.R + cl2.f4.xyz() * cl_a.G + cl3.f4.xyz() * cl_a.B + cl4.f4.xyz() * w4;
		cl_blend.f4.xyz() *= pin.cl.f4.xyz();
		cl_blend.f4.xyz() *= 2.f;
		cl_blend.A = 1.f;
		saturate(cl_blend);
		return cl_blend;
		//return pin.cl;
	}
};

struct lterrain
{
	// 高度图
	
	// 网格
	lmesh<stVPosNorUV>* mesh;

	// 贴图 1234为四个贴图 texalpha为透明贴图，代表每个贴图的透明度 
	ltexture* tex1, *tex2, *tex3, *tex4, *texAlpha;

	lnode_trans trans;

	template <const char* t_vs, const char* t_ps>
	void render( lttri_list_render<t_vs, t_ps>& terrain_render )
	{
		trans.update();
		terrain_render.getvs()->world_trans = &trans;
		terrain_render.getvs()->update();

		terrain_render.getps()->smp1.ptex = tex1;
		terrain_render.getps()->smp2.ptex = tex2;
		terrain_render.getps()->smp3.ptex = tex3;
		terrain_render.getps()->smp4.ptex = tex4;
		terrain_render.getps()->smp_alpha.ptex = texAlpha;

		terrain_render.getps()->uv_scale = 20.f;

		//terrain_render.render( mesh->verts->data(), mesh->verts.size(), mesh->tris->data(),)
		mesh->render(terrain_render);
	}
	
};