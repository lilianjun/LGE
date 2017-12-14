#pragma once

#include <unordered_map>
#include <vector>
#include "lmesh.h"
#include "lsoft_light.h"
#include "lsoft_shader.h"
#include "ltexture.h"
#include "lrender_triangle.h"
#include "lshadow.h"
#include "lwater.h"
#include "lterrain.h"

struct lscene;

struct lmesh_node
{
	lmesh<stVPosNorUV>* mesh;
	ltexture* texture;
	lnode_trans trans;
	
	void update()
	{
		trans.update();
	}

	template <const char* id_vs, const char* id_ps>
	void render(lttri_list_render<id_vs, id_ps>* tri_render)
	{
		if (mesh->verts.size() > 0)
		{
			tri_render->getvs()->world_trans = &trans;
			tri_render->getvs()->update();
			tri_render->getps()->smp.ptex = texture;
			tri_render->render(mesh->verts.data(), mesh->verts.size(), mesh->tris.data(), mesh->tris.size());
		}
		
	}
};

struct lcamera
{
	// 摄像机位置参数
	lview_trans view_trans;
	// 摄像机投影参数
	lproj_trans proj_trans;

	// 更新矩阵
	void update()
	{
		view_trans.update();
		proj_trans.update();
	}
};

struct lscene
{
	std::vector<lmesh_node> nodes;
	lwater water;
	lterrain teraain;
	ltexture* tex_sky;
	lscene_light lit;
	lcamera cam;
	lcolor_buffer clbuf;
	ldepth_buffer depbuf;
	
	lttri_list_render<> tri_render;
	lttri_list_render<lvs_pixel_lit, lps_pixel_lit> tri_render_pixlit;
	// view空间的灯光
	lscene_light lit_in_view;

	// 统计信息
	stRenderStatistics sta;

	// 阴影图
	ldepth_buffer shadow_map;

	// 阴影shader
	lttri_list_render<lvs_shadow_map, lps_shadow_map> shadow_render;

	// 水面渲染器
	lttri_list_render<lvs_water, lps_water> water_render;

	// 地形渲染器
	lttri_list_render<lvs_default, lps_terrain> terrain_render;

	void init()
	{
		// 
		lit_in_view = lit;
		// shader参数的绑定 灯光、摄像机
		tri_render.setbuf(&clbuf, &depbuf);
		tri_render.getvs()->lighter.sclt = &lit_in_view;
		tri_render.getvs()->view_trans = &cam.view_trans;
		tri_render.getvs()->proj_trans = &cam.proj_trans;
		tri_render.getvs()->shiness = 4.f;
		tri_render.psta = &sta;

		tri_render_pixlit.setbuf(&clbuf, &depbuf);
		tri_render_pixlit.getvs()->view_trans = &cam.view_trans;
		tri_render_pixlit.getvs()->proj_trans = &cam.proj_trans;
		tri_render_pixlit.getps()->shiness = 4.f;
		tri_render_pixlit.getps()->lighter.sclt = &lit_in_view;
		tri_render_pixlit.psta = &sta;

		water_render.setbuf(&clbuf, &depbuf);
		water_render.getvs()->view_trans = &cam.view_trans;
		water_render.getvs()->proj_trans = &cam.proj_trans;
		water_render.getps()->shiness = 4.f;
		water_render.getps()->lighter.sclt = &lit_in_view;
		water_render.getps()->view_trans = &cam.view_trans;
		water_render.psta = &sta;

		water.create();

		terrain_render.setbuf(&clbuf, &depbuf);
		terrain_render.getvs()->lighter.sclt = &lit_in_view;
		terrain_render.getvs()->view_trans = &cam.view_trans;
		terrain_render.getvs()->proj_trans = &cam.proj_trans;
		terrain_render.getvs()->shiness = 4.f;
		terrain_render.psta = &sta;
	}

	// 模板参数：三角形列表渲染器
	void render(bool bPixLit = false, bool bShadow = false)
	{
		
		sta.clear();
		// 更新view
		cam.update();

		lit_in_view = lit;

		// 更新光源位置、方向到view空间
		for (int i = 0; i < lit.dir_lts.size(); i++)
		{
			lit_in_view.dir_lts[i].dirction = cam.view_trans.mat.trans_vector(lit.dir_lts[i].dirction).setnor();
		}

		for (int i =0; i < lit.point_lts.size();i++)
		{
			lit_in_view.point_lts[i].pos = cam.view_trans.mat.trans_point(lit.point_lts[i].pos);
		}
		
		clbuf.clear(largb(194, 224, 224));
		depbuf.clear(10000.f);

		// 绘制天空背景
		if (tex_sky && tex_sky->w == clbuf.w && tex_sky->h == clbuf.h)
		{
			memcpy(clbuf.pbuf.data(), tex_sky->buffer.data(), sizeof(largb) * tex_sky->w * tex_sky->h);
		}

		// 逐个渲染
		for (auto & mn : nodes)
		{
			mn.update();
			if (bPixLit)
				mn.render(&tri_render_pixlit);
			else
				mn.render(& tri_render);
		}

		// 渲染地形
		teraain.render(terrain_render);

		// 半透明水面最后渲染
		render_water();
		return;
	}

	// 带阴影的渲染展示
	void render_shadow()
	{
		sta.clear();
		// 更新view
		cam.update();

		// 只有一个点光源
		lit_in_view = lscene_light{ lcolor::white(), {}, 
		{ lpoint_light{float3{0.f, 30.f, 0.f}, lcolor::white(), 1.f, 150.f, .8f, .2f, .0f }, }, .4f, .3f, .3f };

		lit_in_view.point_lts[0].pos = cam.view_trans.mat.trans_point(lit_in_view.point_lts[0].pos);

		clbuf.clear(lcolor::white().get_argb());
		depbuf.clear(1.f);

		lttri_list_render<lvs_shadow_map, lps_shadow_map> draw_shadow_map;

		static ldepth_buffer sm_dp(depbuf.w, depbuf.h);
		
		lview_trans view_lit{ float3(0.f, 15.f, 0.f), float3(-1.f, 0.f, 0.f), float3(0.f, -1.f, 0.f) };
		lproj_trans proj_lit{ 3.f / 4.f, L_PIDIV2, 1.f, 1000.f, 800, 600 };

		view_lit.update();
		proj_lit.update();
		draw_shadow_map.setbuf(&clbuf, &sm_dp);
		draw_shadow_map.getvs()->view_trans = &view_lit;
		draw_shadow_map.getvs()->proj_trans = &proj_lit;
		draw_shadow_map.psta = &sta;

		clbuf.clear(lcolor::white().get_argb());
		depbuf.clear(1.f);
		sm_dp.clear(1.f);
		
		// 绘制阴影图时不裁剪背面
		draw_shadow_map.bback_cull = false;

		// 逐个渲染
		for (auto & mn : nodes)
		{
			mn.update();
			mn.render(&draw_shadow_map);
		}

		// 渲染完成得到阴影图 下面开始渲染阴影
		lttri_list_render<lvs_receive_shadow, lps_receive_shadow> draw_shadowed_scene;

		draw_shadowed_scene.setbuf(&clbuf, &depbuf);
		draw_shadowed_scene.psta = &sta;

		draw_shadowed_scene.getvs()->view_trans = &cam.view_trans;
		draw_shadowed_scene.getvs()->proj_trans = &cam.proj_trans;
		draw_shadowed_scene.getvs()->view_lit = &view_lit;
		draw_shadowed_scene.getvs()->proj_lit = &proj_lit;

		draw_shadowed_scene.getps()->lighter.sclt = &lit_in_view;
		draw_shadowed_scene.getps()->shiness = 4.f;
		draw_shadowed_scene.getps()->shadow_map = &sm_dp;
		
		clbuf.clear(largb(194, 224, 224));
		depbuf.clear(1.f);
		// 逐个渲染
		for (auto & mn : nodes)
		{
			mn.update();
			mn.render(&draw_shadowed_scene);
		}

	}

	// 渲染水面
	void render_water()
	{
		water.render(water_render);
	}
};

inline lmesh<stVPosNorUV>* getmesh(const std::string& name)
{
	static std::unordered_map<std::string, lmesh<stVPosNorUV>* > mesh_map;

	auto it = mesh_map.find(name);
	if (it != mesh_map.end())
	{
		return it->second;
	}
	else
	{
		lmesh<stVPosNorUV>* pmesh = new lmesh<stVPosNorUV>();
		if (!pmesh->load(name.c_str()))
		{
			delete pmesh;
			pmesh = nullptr;
			return nullptr;
		}
		else
		{
			mesh_map[name] = pmesh;
			return pmesh;
		}

	}
}

inline ltexture* gettexture(const std::string& name)
{
	static std::unordered_map<std::string, ltexture*> tex_map;

	auto it = tex_map.find(name);
	if (it != tex_map.end())
	{
		return it->second;
	}
	else
	{
		auto ptex = new ltexture();
		if (!lload_bmp(name.c_str(), *ptex))
		{
			delete ptex;
			ptex = nullptr;
			return nullptr;
		}
		else
		{
			tex_map[name] = ptex;
			return ptex;
		}
	}
}



	

	

