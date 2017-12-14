#pragma once

// ��Ⱦ�������㷨
#include "lmath.hpp"
#include "lsoft_shader.h"
#include "lvertex_lerper.h"

// ��Ⱦͳ��
struct  stRenderStatistics
{
	int ntri_total;
	int ntri_back_cull;
	int ntri_out_cull;
	int nver;
	int nmesh;
	int npixel;
	int npixel_depth_passed;

	void clear()
	{
		memset(this, 0, sizeof(stRenderStatistics));
	}
};

//////////////////////////////////////////////////////////////////////////
// ���������βü���
// ���죺�ж��淨�������ߵķ���
// ������μ����أ�ȡ������е㣬��ԭ��
struct ltri_back_culler
{
	// ���룺��������
	// ������Ƿ��Ǳ���
	// �Ƿ��Ǳ���
	inline bool operator()(const float* p1, const float* p2, const float* p3)
	{
		// �������η��ߣ�
		// cross: (p2 - p1).nor, (p3 - p1).nor
		float2 v1{ p1[0], p1[1] };
		float2 v2{ p2[0], p2[1] };
		float2 v3{ p3[0], p3[1] };

		// ��ά�У��������㣬���ߵ��ڲ������� ��2dcoross����
		auto p = (v2 - v1).cross(v3 - v1);
		return p > 0;
	}
};

//////////////////////////////////////////////////////////////////////////
// ����βü��� ������ġ�
// ���룺���б� ������㷨�������㷨
template <int nfloat>
struct lpologon_culler
{
	typedef lfloatn<nfloat> t_point;
	typedef std::list<t_point> t_point_list;

	template <typename t_isinside, typename t_intersection >
	inline bool operator()(t_point_list& point_list, t_isinside isindide, t_intersection intersection)
	{
		if (point_list.size() < 3) return 0;
		auto it = point_list.begin();
		auto it2 = it;

		t_point_list list_out;

		// 
		while ( it != point_list.end())
		{
			it2 = it;
			it2++;
			it2 = (it2 == point_list.end() ? point_list.begin() : it2);

			bool bp1 = isindide(*it);
			bool bp2 = isindide(*it2);

			if (bp1 && bp2)
			{
				// ���㶼���� ���p1
				list_out.push_back(*it);
				it++;
			}
			else if (bp1 && (!bp2))
			{

				// ���p1�ͽ���
				list_out.push_back(*it);
				list_out.emplace_back(intersection(*it, *it2));
				it++;
			}
			else if ((!bp1) && bp2)
			{
				// p1���� p2���� �������
				list_out.emplace_back(intersection(*it, *it2));
				it++;
			}
			else if ((!bp1) && (!bp2))
			{
				it++;
			}
		}
		point_list.swap(list_out);
		return (point_list.size() > 2);
	}

	template <typename t_isinside , typename t_intersection >
	inline bool operator()(t_point_list& point_list, t_isinside isindide, t_intersection intersection, int xx)
	{
		if (point_list.size() < 3) return 0;
		auto it = point_list.begin();
		auto it2 = it;
		
		t_point_list list_out;

		// 
		while (point_list.size() > 2 && it != point_list.end())
		{
			it2 = it;
			it2++;
			it2 = (it2 == point_list.end() ? point_list.begin() : it2);

			bool bp1 = isindide(*it);
			bool bp2 = isindide(*it2);

			if (bp1 && bp2)
			{
				// ���㶼���� ����p1 ��һ�δ�p2��ʼ
				it++;
			}
			else if (bp1 && (!bp2))
			{
				
				// it2�ǵ�һ������������it2ɾ��
				if (it2 == point_list.begin())
				{
					// ����������
					point_list.emplace(point_list.end(), intersection(*it, *it2));
					point_list.erase(it2);
				}
				else
				{
					// p1�ڣ� p2���� ����p1 �� ����zmax�Ľ��� ����������p1�ĺ���
					// ��һ����P2��ʼ
					it = point_list.emplace(it2, intersection(*it, *it2)); //
					it++;
				}
			}
			else if ((!bp1) && bp2)
			{
				// p1���� p2���� �������
				// ���㽻�� ɾ��p1�㣬p2ǰ����p1�� ��һ����p2�㿪ʼ ��Ϊp1 p2�������ڵ���
				//outp.push_back(tl(p1, p2).pz(pmax.z));
				auto ins = intersection(*it, *it2);
				if (it != point_list.begin())
				{
					it = point_list.erase(it); //
					it = point_list.insert(it, ins); // itָ�򽻵�
					it++;
				}
				else
				{
					// �ǵ�һ���������� ���ڵڶ�����ǰ��
					it = point_list.insert(it2, ins);
					it++;
				}
				
			}
			else if ((!bp1) && (!bp2))
			{
				// p1 ��������� �Ͳ�ɾ��p1��Ҳ��ɾp2
				if (it == point_list.begin() )
				{
					it++;
				}
				// p2�����������˵�����յ��� �������㶼ɾ��
				else if (it2 == point_list.begin())
				{
					point_list.erase(it);
					point_list.erase(it2);
					it = point_list.end();
				}
				else
				{
					// ���㶼���⣬ɾ��P1�� ��һ�δ�p2�㿪ʼ itָ����p2�� ����++
					it = point_list.erase(it); // 
				}
			}
		}
	
		return (point_list.size() > 2);
	}
};

//////////////////////////////////////////////////////////////////////////
// �������βü�����
// ���죺�ü���box
// ���룺ͶӰ����������� 
// ������ü����0~n��������
// ģ�������nfloat �����float������
template <int nfloat>
struct ltri_box_culler
{
	lboxf box;

	typedef lfloatn<nfloat> t_vertex;
	// �ü���ĵ������ֵҲ����Ҫ��ֵ��

	// �ü����
	std::list<t_vertex> tri_list;

	void operator()(const float* p1, const float* p2, const float* p3)
	{
		tri_list.clear();

		// ����ü�
		if (!ltri_back_culler()(p1, p2, p3))
		{
			return;
		}

		// Z�ü� һ���������е���[0, 1]��Χ�⣬��ֱ�������������� ��Ϊ��̫������̫Զ���ü�Ҳû������
		if (p1[2] < box.pmin.z || p1[2] > box.pmax.z
			|| p2[2] < box.pmin.z || p2[2] > box.pmax.z
			|| p3[2] < box.pmin.z || p3[2] > box.pmax.z)
		{
			return;
		}

		tri_list = { lmake_floatn<nfloat>(p1), lmake_floatn<nfloat>(p2), lmake_floatn<nfloat>(p3) };

		lpologon_culler<nfloat> cull;

		// xmin�ü� �뿪�߽�һ�����غ������ü�
		auto cullminx = [this](const t_vertex& v) {return v[0] > box.pmin.x - 1.f;};
		auto interminx = [this](const t_vertex& v1, const t_vertex& v2)
		{ return llinen<float, nfloat>{v1, v2}.point(0, box.pmin.x); };
		if (!cull(tri_list, cullminx, interminx)) return;

		// x max�ü�
		auto cullmaxx = [this](const t_vertex& v) {return v[0] < box.pmax.x + 1.f;};
		auto intermaxx = [this](const t_vertex& v1, const t_vertex& v2)
		{ return llinen<float, nfloat>{v1, v2}.point(0, box.pmax.x); };
		if (!cull(tri_list, cullmaxx, intermaxx)) return;

		// y min �ü�
		auto cullminy = [this](const t_vertex& v) {return v[1] > box.pmin.y - 1.f;};
		auto interminy = [this](const t_vertex& v1, const t_vertex& v2)
		{ return llinen<float, nfloat>{v1, v2}.point(1, box.pmin.y); };
		if (!cull(tri_list, cullminy, interminy)) return;

		// y max�ü�
		auto cullmaxy = [this](const t_vertex& v) {return v[1] < box.pmax.y + 1.f;};
		auto intermaxy = [this](const t_vertex& v1, const t_vertex& v2)
		{ return llinen<float, nfloat>{v1, v2}.point(1, box.pmax.y); };
		if (!cull(tri_list, cullmaxy, intermaxy)) return;

		// z�ü�
	}
};

//////////////////////////////////////////////////////////////////////////
// ��Ļ�ռ������βü���
// ���죺rect
// ���룺������������
// ������ü����0~n��������
template <int nfloat>
struct ltri_rect_culler
{
	lrectf rect;

	typedef lfloatn<nfloat> t_point;

	bool operator()(std::list<t_point>& tri_list)
	{
		lpologon_culler<nfloat> cull;

		// xmin�ü� 
		auto cullminx = [this](const t_point& v) {return !(v[0] < rect.pmin.x);};
		auto interminx = [this](const t_point& v1, const t_point& v2)
		{ return lpoint_on_line<float, nfloat>(v1, v2, 0, rect.pmin.x); };
		if (!cull(tri_list, cullminx, interminx)) return false;

		// x max�ü�
		auto cullmaxx = [this](const t_point& v) {return !(v[0] > rect.pmax.x );};
		auto intermaxx = [this](const t_point& v1, const t_point& v2)
		{ return lpoint_on_line<float, nfloat>(v1, v2, 0, rect.pmax.x); };
		if (!cull(tri_list, cullmaxx, intermaxx)) return false;

		// y min �ü�
		auto cullminy = [this](const t_point& v) {return !(v[1] < rect.pmin.y);};
		auto interminy = [this](const t_point& v1, const t_point& v2)
		{ return lpoint_on_line<float, nfloat>(v1, v2, 1, rect.pmin.y); };
		if (!cull(tri_list, cullminy, interminy)) return false;

		// y max�ü�
		auto cullmaxy = [this](const t_point& v) {return !(v[1] > rect.pmax.y);};
		auto intermaxy = [this](const t_point& v1, const t_point& v2)
		{ return lpoint_on_line<float, nfloat>(v1, v2, 1, rect.pmax.y); };
		if (!cull(tri_list, cullmaxy, intermaxy)) return false;
		
		return true;
	}
};

//////////////////////////////////////////////////////////////////////////
// ������Z�ü���
// ����:neraZ farZ
// ���룺��near farͬһ�ռ��������
// ������ü����������
template <size_t nfloat>
struct ltri_zculler
{
	float nearZ, farZ;

	typedef lfloatn<nfloat> t_point;

	bool operator()(std::list<lfloatn<nfloat> >& tri_list)
	{
		lpologon_culler<nfloat> cull;

		// zmin�ü�
		auto cullminz = [this](const t_point& v) {return !(v[2] < nearZ);};
		auto interminz = [this](const t_point& v1, const t_point& v2)
		{ return lpoint_on_line(v1, v2, 2, nearZ); };
		if (!cull(tri_list, cullminz, interminz)) return false;

		// zmax�ü�
		auto cullmaxz = [this](const t_point& v) {return !(v[2] > farZ);};
		auto intermaxz = [this](const t_point& v1, const t_point& v2)
		{ return lpoint_on_line(v1, v2, 2, farZ); };
		if (!cull(tri_list, cullmaxz, intermaxz)) return false;

		return true;
	}
};

//////////////////////////////////////////////////////////////////////////
// �������ι�դ������
// ���룺ͶӰ����������� ���������ζ���
// ���죺���ش������
// ����������ش��������
template <const char* id_ps>
struct ltri_raster
{
	static stRenderStatistics* psta;

	using t_ps = ltps<id_ps>;
	using t_vertex = typename t_ps::t_in;
	t_ps ps;
	lcolor_buffer* clbuf;
	ldepth_buffer* dpbuf;

	bool bblend;

	inline void operator()(t_vertex* pin)
	{
		// �������� ��դ�� ��ֵ����
		// Ȼ���ÿ������PS�������ɫֵ
		this->operator()(pin, pin + 1, pin + 2);
	}

	inline void operator()(t_vertex* p1, t_vertex* p2, t_vertex* p3)
	{

		float* v1 = (float*)p1;
		float* v2 = (float*)p2;
		float* v3 = (float*)p3;

		// X Y��ȡ����
		v1[0] = lfround(v1[0]);
		v1[1] = lfround(v1[1]);
		v2[0] = lfround(v2[0]);
		v2[1] = lfround(v2[1]);
		v3[0] = lfround(v3[0]);
		v3[1] = lfround(v3[1]);

		// �������� Y��С����
		if (v1[1] > v2[1])
		{
			lswap(v1, v2);
		}
		if (v2[1] > v3[1])
		{
			lswap(v2, v3);
		}
		if (v1[1] > v2[1])
		{
			lswap(v1, v2);
		}

		const size_t _nflt = sizeof(t_vertex) / sizeof(float);
		lvertex_steperY<_nflt> l12, l13, l23;

		l12.beg(v1, v2);
		l13.beg(v1, v3);
		l23.beg(v2, v3);
		auto ptemp = &l12;
		while (!l13.end())
		{
			if (l12.end())
			{
				ptemp = &l23;
			}
			auto pL = ptemp->cur.data();
			auto pR = l13.cur.data();
			// ��֤Y��ͬ��Ϊ����
			//pf2L[1] = ptemp->ai;
			//pf2R[1] = l13.ai;
			assert( lequal( pL[1],  pR[1]) );
			if (pL[0] > pR[0])
			{
				lswap(pL, pR);
			}
			lvertex_steperX<_nflt> llr;
			// ��XҲ�л�Ϊ����
			llr.beg(pL, pR);
			llr.p1[0] = lfround(llr.p1[0]);
			llr.p2[0] = lfround(llr.p2[0]);
			
			while (!llr.end())
			{
				// �����llr.bi
				const float3* pos = (float3*)llr.cur.data();
				int x = liround(pos->x);
				int y = liround(pos->y);
				psta->npixel++;

				if (y < 10)
				{
					x += 1;
				}

				// ��Ȳ���
				if (dpbuf->test(x, y, pos->z))
				{
					auto cl = ps.process(*((typename t_ps::t_in*)llr.cur.data()));
					
					if (bblend)
					{
						// ͸���Ȳ���
						if (cl.A >  0.0001f)
						{
							lcolor cl2 = lcolor::create(clbuf->get(x, y));
							cl2.f4.xyz() = cl2.f4.xyz().lerp(cl.f4.xyz(), 1.f - cl.A);
							clbuf->set(x, y, cl2.get_argb());
							dpbuf->set(x, y, pos->z);
						}
						
					}
					else
					{
						clbuf->set(x, y, cl.get_argb());
						dpbuf->set(x, y, pos->z);
					}
				}
				else
				{
					psta->npixel_depth_passed++;
				}
				llr.next();
			}

			// �м�һ�л����� 13������
			ptemp->next();
			if ( ptemp == &l12 )
			{
				if(!l12.end())
					l13.next();
			}
			else
				l13.next();
			
		}
	}
};

template <const char* id_ps>
stRenderStatistics* ltri_raster<id_ps>::psta = nullptr;



//////////////////////////////////////////////////////////////////////////
// ���������б���Ⱦ����
// ���죺�����������س���
// ���룺��������
// ������������س�����ִ�����
template <const char* id_vs = lvs_default, const char* id_ps = lps_default>
struct lttri_list_render
{
	static stRenderStatistics* psta;

	// ���죺������ɫ�� ������ɫ�� ͶӰ��
	using t_vs = ltvs<id_vs>;
	using t_ps = ltps<id_ps>;

	typedef typename t_vs::t_out t_vsout;

	bool bdep_test;
	bool bdep_write;
	bool bback_cull;
	// �Ƿ���͸�����
	bool bblend;

	lttri_list_render():bdep_test(true), bdep_write(true), bback_cull(true), bblend(true) {}

	lttri_list_render(lcolor_buffer* _clbuf, ldepth_buffer* _dpbuf)
		:bdep_test(true), bdep_write(true), bback_cull(true), bblend(true)
	{
		raster.clbuf = _clbuf;
		raster.dpbuf = _dpbuf;
	}

	// ��ȡ��ɫ��
	t_vs* getvs() { return &vs; }
	t_ps* getps() { return &raster.ps; }

	void setbuf(lcolor_buffer* _clbuf, ldepth_buffer* _dpbuf)
	{
		raster.clbuf = _clbuf;
		raster.dpbuf = _dpbuf;
	}

	// ��Ⱦ���� ���룺�������� ��������
	void render(const typename t_vs::t_in* pvsin, int nver, const lint3* pindex, int ntri)
	{
		psta->nmesh += 1;
		psta->nver += nver;
		psta->ntri_total += ntri;
		raster.psta = psta;
		raster.bblend = bblend;

		// ���������׶Ρ�
		vsout.resize(nver);
		for (int i = 0; i < nver; i++)
		{
			vsout[i] = vs.process(pvsin[i]);
		}
		t_vsout* prin = vsout.data();
		
		// ��դ���׶�
		const auto nflt = sizeof(t_vsout) / sizeof(float);

		ltri_zculler<nflt> zcull{vs.proj_trans->zn, vs.proj_trans->zf};

		ltri_rect_culler<nflt> rectcull{ lrectf{0, 0, vs.proj_trans->w_vp, vs.proj_trans->h_vp}  };

		ltri_back_culler backcull;

		for (int i = 0; i < ntri; i++)
		{

			auto p1 = prin + pindex[i].x;
			auto p2 = prin + pindex[i].y;
			auto p3 = prin + pindex[i].z;

			std::list<lfloatn<nflt>> tri_list{ lmake_floatn<nflt>((const float*)p1),
				lmake_floatn<nflt>((const float*)p2), lmake_floatn<nflt>((const float*)p3) };

			// TODO:����ü��ɷ�������׶�

			// Z�ü�
			if (!zcull(tri_list))
			{
				psta->ntri_out_cull += 1;
				continue;
			}

			// ͶӰ�����ζ���
			for (auto& point : tri_list)
			{
				float3* pv = (float3*)&point;
				*pv =  vs.proj_trans->trans(*pv);
			}

			// ��Ļ�ü�
			if (!rectcull(tri_list))
			{
				psta->ntri_out_cull += 1;
				continue;
			}

			// ��դ��
			// �Ӳü������ȡ�����е������ν��л���
			auto it1 = tri_list.begin();
			auto it2 = it1; it2++;
			auto it3 = it2; it3++;

			while (it3 != tri_list.end())
			{
				// ����ü�
				if ( !bback_cull || ( bback_cull && backcull(it1->data(), it2->data(), it3->data())))
				{
					raster((t_vsout*)&*it1, (t_vsout*)&*it2, (t_vsout*)&*it3);
					it2 = it3;
				}
				else
				{
					psta->ntri_back_cull += 1;
				}
				
				it3++;
			}
		}
	}

private:
	// �������
	t_vs vs;
	// �ݴ��м�����
	std::vector<t_vsout> vsout;
	// �����ι�դ����
	ltri_raster<id_ps> raster;
};

 template <const char* id_vs /*= lvs_default*/, const char* id_ps /*= lps_default*/>
stRenderStatistics* lttri_list_render<id_vs, id_ps>::psta = nullptr;
