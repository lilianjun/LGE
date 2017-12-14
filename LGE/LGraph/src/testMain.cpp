#include <iostream>
#include <string>
#include <windows.h>
#include <sstream>

#include <stack>

#include "LImageBuffer.h"
#include "LCanvas.h"

#include "LTriDrawer.h"

#include "LSoftGraph.h"

#include "LTimer.h"
#include <map>
#include "lgraph.hpp"
#include <array>
#include "lvectorn.h"
#include "lrender_line.h"
#include "lmesh.h"
#include "lsoft_shader.h"
#include "ltexture.h"
#include "lscene.h"

const static LPTSTR szClass = TEXT("LGraphWnd");
const static LPTSTR szTitle = TEXT("LGraph");

LImageBuffer* pimage = nullptr;
std::vector<unsigned int> imageData;

LSoftGraph graph;

POINT lastMousePos = {-1, -1};

bool bLineBeg = false;

DWORD g_cl = lcolor::red().get_argb();

LColorBuffer g_surface;

std::stack<POINT> g_points;

class CTimer;

CTimer g_timer;

LBrushEx g_brush({ 
			{.3f, .3f, 0.3f}, 
			{.3f, 1.f, .3f}, 
			{.3f, .3f, .3f} });

enum EDrawMode
{
	eNull, ePoint, eLine, eRect, eTrianlge, eCircle
};

EDrawMode drawMode = eNull;

// 线的宽度
int g_lineWidth = 1;

// 使用笔刷
int g_brushID = 1;

// 是否填充
bool g_fill = true;

//窗口过程
LRESULT CALLBACK __WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);


lscene* g_scene = nullptr;

inline largb randcl()
{
	return largb(lrand(0, 255), lrand(0, 255), lrand(0, 255));
}

// 运算速度的测试
void test_speed()
{
	// float乘法的效率的测试
	float fa1[10000];
	float fa2[10000];
	int ia1[10000];
	int ia2[10000];

	for (int i = 0; i < 10000; i++)
	{
		fa1[i] = lrandf();
		ia1[i] = lrand();
	}

	
	CTimer timer;
	std::map<double, std::string> tmap;
	// null loop
	timer.Start();
	for (int j = 0; j < 100000; j++)
	{
		for (int i = 0; i < 10000; i++)
		{
			ia2[i] = ia1[i];
		}
	}
	auto t_null_loop = timer.Stop();
	tmap[t_null_loop] = "t_null_loop";

	// 浮点数乘法
	timer.Start();
	for (int j = 0; j < 100000; j++)
	{
		for (int i = 0; i < 10000; i++)
		{
			fa2[i] = fa1[i] * 3.3333f;
		}
	}
	auto t_flt_mul  = timer.Stop();
	tmap[t_flt_mul] = "t_float_mul";

	// 浮点数加法
	timer.Start();
	for (int j = 0; j < 100000; j++)
	{
		for (int i = 0; i < 10000; i++)
		{
			fa2[i] = fa1[i] + 3.3333f;
		}
	}
	auto t_float_add = timer.Stop();
	tmap[t_float_add] = "t_float_add";

	// 浮点数除法
	timer.Start();
	for (int j = 0; j < 100000; j++)
	{
		for (int i = 0; i < 10000; i++)
		{
			fa2[i] = fa1[i] / 3.3333f;;
		}
	}
	auto t_float_div = timer.Stop();
	tmap[t_float_div] = "t_float_div";

	// 

	timer.Start();
	for (int j = 0; j < 100000; j++)
	{
		for (int i = 0; i < 10000; i++)
		{
			ia2[i] = ia1[i] * 100;
		}
	}
	auto t_int_mul = timer.Stop();
	tmap[t_int_mul] = "t_int_mul";

	timer.Start();
	for (int j = 0; j < 100000; j++)
	{
		for (int i = 0; i < 10000; i++)
		{
			ia2[i] = ia1[i] + 37113;
		}
	}
	auto t_int_add = timer.Stop();
	tmap[t_int_add] = "t_int_add";

	timer.Start();
	for (int j = 0; j < 100000; j++)
	{
		for (int i = 0; i < 10000; i++)
		{
			ia2[i] = ia1[i] / 1113;
		}
	}
	auto t_int_div = timer.Stop();
	tmap[t_int_div] = "t_int_div";

	for (auto v : tmap)
	{
		std::cout << v.second << "\t" << v.first << std::endl;
	}

	//std::cout << "float乘法 \\ int乘法 " << t_flt_mul << "\\" << t_int_mul << " = " << t_flt_mul / t_int_mul << std::endl;

	//std::cout << "float加法 \\ int加法 " << t_flt_mul << "\\" << t_int_mul << " = " << t_int_mul / t_flt_mul << std::endl;

}

void Frame();

void Render(HWND hwnd);

void Destroy();

void Init();

void ShowPic(const void* pic, int w, int h, HWND hwnd);

int main(int argc, char *argv[])
{

	HINSTANCE hIns = ::GetModuleHandle(0);
	WNDCLASSEX wc;
	wc.cbSize = sizeof(wc);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hIns;
	wc.hIcon = LoadIcon(0, IDI_APPLICATION);
	wc.hIconSm = 0;
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.hCursor = LoadCursor(0, IDC_ARROW);
	wc.lpfnWndProc = __WndProc;
	wc.lpszMenuName = NULL;
	wc.lpszClassName = szClass;

	if (!RegisterClassEx(&wc)) exit(0);

	DWORD style = WS_OVERLAPPEDWINDOW;
	DWORD styleEx = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;

	//计算客户区域为宽800,高600的窗口尺寸
	RECT rect = { 0, 0, 800, 600 };
	AdjustWindowRectEx(&rect, style, false, styleEx);

	//pimage = new LImageBuffer(rect.right - rect.left, rect.bottom - rect.top);
	

	HWND hWnd = CreateWindowEx(styleEx, szClass, szTitle, style, 0, 0,
		rect.right - rect.left, rect.bottom - rect.top, 0, 0, hIns, 0);
	if (hWnd == 0) exit(0);

	//graph.Create(hWnd, 800, 600);
	//graph.Clear(largb::gray(0.2f));

	//g_surface.create(800, 600);
	//g_surface.clear(largb::gray(0.2f));
	//g_pointConvert.Create(hWnd, 800, 600);
	graph.Create(hWnd, 800, 600);
	graph.ClearColor(largb::gray(0.5f));
	graph.ClearDepth(1.0f);
	
	Init();

	UpdateWindow(hWnd);
	ShowWindow(hWnd, SW_SHOW);

	//TODO, init this
	Render(hWnd);

	MSG msg = { 0 };
	while (msg.message != WM_QUIT)
	{
		
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			Render(hWnd);
		}
		else
		{
			//TODO, do this
			//auto cl = RandCl();
			//graph.Clear(RandCl());
			//graph.Present();
			//Frame();
			Render(hWnd);
		}
	}

	return 0;
}

// 
float3 g_pos(0.f, 0.f, -20.f);
float3 g_rot(0.f, 0.f, 0.f);
float3 g_scl(1.f, 1.f, 1.f);

float3 g_camPos;
float3 g_camUp;


// 顶点结构
struct stVSIn
{
	float3 pos;
	largb cl;
};

struct stVSOut 
{
	float3 pos;
	lcolor cl;
};

// 顶点投影器 给出宽高
struct lproj_wh
{
	// 构造：长、宽、近裁面、 远裁面
	float w,  h,  n,  f;
	float bufW, bufH;

	void operator()(void* _pv)
	{
		float3* pv = (float3*)_pv;
		auto tempz = max( abs(pv->z), 2.f/ w);

		pv->x = ( pv->x *2 * n / w / pv->z + 1.f) * bufW / 2; //(pv->x *  n / tempz + 1.f) * w / 2;
		pv->y = ( pv->y * 2 * n / h / pv->z + 1.f) * bufH / 2;
		
		//Zfar / (Zfar - Znear) * z + Znear*Zfar / (Znear - Zfar), 0);
		pv->z = (f * pv->z - n * f) / (f - n);
	}
};

struct lproj_fov
{
	float aspect, ridio, n, f;
};

// 顶点着色器
struct l_vs
{
	typedef stVSIn t_in;
	typedef stVSOut t_out;
	lmatrix4f mat;
	int w, h;

	lproj_wh pro;

	stVSOut operator () (const stVSIn& ver)
	{
		// 转换到view空间
		auto v =  mat.trans_point( float4( ver.pos, 1.f) );
		// 进行其他操作

		return stVSOut{ v.xyz(),  lcolor::create(ver.cl) };
	}
};

struct l_ps
{
	lcolor_buffer target;
	ldepth_buffer depth;
	typedef stVSOut t_in;
	void operator()(const stVSOut& p)
	{
		//ptar[w * (int)p.y + (int)p.x] = largb::red();
		int x = (liround(p.pos.x));
		int y = ( liround(p.pos.y));// *ptar->GetH());
		if (depth.test(x, y, p.pos.z))
		{
			//target.set(x, y, (pin.cl * texcl ).setclamp(0.f, 1.f).get_argb());
			//target.set(x, y, texcl.get_argb());
			target.set(x, y, p.cl.get_argb());
			depth.set(x, y, p.pos.z);
		}
	}
};

// 网格
struct grid 
{
	grid( )
	{
		for (int i = -10; i < 11; i++)
		{
			// x:-10 到10 z:10, -10
			vers.emplace_back(stVSIn{ float3(i, 0, 10), largb::gray() });
			vers.emplace_back(stVSIn{ float3(i, 0, -10), largb::gray() });
			// 
			vers.emplace_back(stVSIn{float3(-10, 0, i), largb::gray()});
			vers.emplace_back(stVSIn{ float3(10, 0, i), largb::gray() });
		}
	}

	void render(l_vs vs, l_ps ps, lproj_wh proj)
	{
		lrender_line_list<l_vs, l_ps, lproj_wh> m_render{ vs, ps, proj };
		m_render(vers.data(), vers.size() / 2);
	}

private:
	std::vector<stVSIn> vers;
	
};

std::vector<stVSIn> ver = {
	{ {0.f, 0.f, 0.f},	largb::red()},
	{ {.5f,0.f, 0.f},	largb::green()},
	{ {.5f, .5f, 0.f},largb::blue()},
	{ {0.f, .5f, 0.f},	largb::gray()}
	};

std::vector<lint3> index = {
	lint3(0, 1, 2), lint3(0, 2, 3) };
std::vector<largb> cl = { largb::red(), largb::green(), largb::blue(), largb::white() };

grid g_grid;

float3 g_light_dir{ 1.f, 0.f, 0.f };

lmesh<stVPosNorUV> g_mesh;

lcolor_buffer g_tarbuf;
ldepth_buffer g_depbuf;

ltexture g_texcat1;

// 图片绘制到窗口
void ShowPic(const void* pic, int w, int h, HWND hwnd)
{
	BITMAPINFO m_BmpInfo;
	memset(&m_BmpInfo, sizeof(m_BmpInfo), 0);
	m_BmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	m_BmpInfo.bmiHeader.biWidth = w;
	m_BmpInfo.bmiHeader.biHeight = h;
	m_BmpInfo.bmiHeader.biPlanes = 1;
	m_BmpInfo.bmiHeader.biBitCount = 32;
	m_BmpInfo.bmiHeader.biCompression = BI_RGB;
	m_BmpInfo.bmiHeader.biSizeImage = 0;
	m_BmpInfo.bmiHeader.biXPelsPerMeter = 0;
	m_BmpInfo.bmiHeader.biYPelsPerMeter = 0;
	m_BmpInfo.bmiHeader.biClrUsed = 0;
	m_BmpInfo.bmiHeader.biClrImportant = 0;

	RECT rect;
	GetClientRect(hwnd, &rect);
	HDC hdc = GetDC(hwnd);
	StretchDIBits(hdc, 0, 0, rect.right - rect.left, rect.bottom - rect.top,
		0, 0, w, h, pic,
		&m_BmpInfo, DIB_RGB_COLORS, SRCCOPY);
	ReleaseDC(hwnd, hdc);
}


lmesh<stVPosNorUV> g_box, g_sphere, g_teapot, g_capsule, g_pyramind, g_torus;
lmesh<stVPosNorUV> g_cylinder, g_cone, g_ChamferBox, g_plane;

ltexture g_texcat2;
ltexture g_texcat3;
ltexture g_tex_Terrain_grass_12;

template <const char* str>
struct myst
{
	 myst() { std::cout << "myst<>"<<std::endl; }
};

extern const char myst_1[] = "myst_1";
template <>
struct myst<myst_1>
{
	int v;
	myst(int v) { std::cout << "myst<int>"<<v<<std::endl; }
};



void Init()
{
	// 加载资源

// 	bool bloadmesh = g_mesh.load("res/Cat_yellow.lmesh");
// 	assert(bloadmesh);
// 	bloadmesh = g_box.load("res/box.lmesh");
// 	assert(bloadmesh);
// 	bloadmesh = g_sphere.load("res/Sphere001.lmesh");
// 	assert(bloadmesh);
// 	bloadmesh = g_capsule.load("res/Capsule001.lmesh");
// 	assert(bloadmesh);
// 	bloadmesh = g_teapot.load("res/Teapot001.lmesh");
// 	assert(bloadmesh);
// 	bloadmesh = g_pyramind.load("res/pyramind.lmesh");
// 	assert(bloadmesh);
// 	bloadmesh = g_torus.load("res/Torus001.lmesh");
// 	assert(bloadmesh);
// 	bloadmesh = g_cylinder.load("res/cylinder.lmesh");
// 	assert(bloadmesh);
// 	bloadmesh = g_cone.load("res/cone.lmesh");
// 	assert(bloadmesh);
// 	bloadmesh = g_ChamferBox.load("res/chamferBox.lmesh");
// 	assert(bloadmesh);
// 	bloadmesh = g_plane.load("res/Plane001.lmesh");
// 	assert(bloadmesh);
// 
// 	bool bloadtex = lload_bmp("res/cat1.bmp", g_texcat1);
// 	assert(bloadtex);
// 	bloadtex = lload_bmp("res/catwhite.bmp", g_texcat2);
// 	assert(bloadtex);
// 	bloadtex = lload_bmp("res/catsham.bmp", g_texcat3);
// 	assert(bloadtex);
// 	bloadtex = lload_bmp("res/Terrain_grass_12.bmp", g_tex_Terrain_grass_12);

	// 创建创景
	g_scene = new lscene{ 

		// 场景节点构造
		std::vector<lmesh_node>{
			{ getmesh("res/BLJ_01.lmesh"), gettexture("res/BaiLuJing_01.bmp"),
				{ { -5.f, 0.f, 0.f },{ 0.f, 0.f, 0.f },{ .5f, .5f, .5f } } },
			//{ getmesh("res/Sphere001.lmesh"), gettexture("res/terrain_rock_003.bmp"),	
			//	{ { 0.f, 0.f, 0.f },{ 0.f, 0.f, 0.f },{ 1.f, 1.f, 1.f } } },
			//{ getmesh("res/Capsule001.lmesh"), nullptr,
			//	{ { 0.f, 0.f, 5.f },{ 0.f, 0.f, 0.f },{ 1.f, 1.f, 1.f } } },
			//{ getmesh("res/Teapot001.lmesh"), gettexture("res/Terrain_road_014.bmp"),	
			//	{ { 5.f, 0.f, 0.f },{ 0.f, 0.f, 0.f },{ 1.f, 1.f, 1.f } } },
			//{ &g_pyramind, nullptr,	{ { 5.f, 0.f, 5.f },{ 0.f, 0.f, 0.f },{ 1.f, 1.f, 1.f } } },
			//{ getmesh("res/Torus001.lmesh"), gettexture("res/Terrain_road_015.bmp"),	
			//	{ { -5.f, 2.5f, 0.f },{ 0.f, 0.f, 0.f },{ 1.f, 1.f, 1.f } } },
			//{ &g_cylinder, nullptr,	{ { -5.f, 0.f, 5.f },{ 0.f, 0.f, 0.f },{ 1.f, 1.f, 1.f } } },
			//{ &g_cone, nullptr,		{ { 0.f, 0.f, 10.f },{ 0.f, 0.f, 0.f },{ 1.f, 1.f, 1.f } } },
			//{ &g_ChamferBox, nullptr,{ { 5.f, 0.f, 10.f },{ 0.f, 0.f, 0.f },{ 1.f, 1.f, 1.f } } },
		},

		// 水面构造
		lwater{ getmesh("res/water_plane.lmesh"),
				gettexture("res/water_normal.bmp"),
				gettexture("res/Water fallback.bmp"),
		{{ -10.f, -3.f, -40.f },{ 0.f, 0.f, 0.f },{ 5.f, 5.f, 5.f } } },

			// 地形构造
		lterrain{ getmesh("res/terrain64.lmesh"),
			gettexture("res/terrain_grass_001.bmp"),
			gettexture("res/terrain_road_003.bmp"),
			gettexture("res/Terrain_rock_014.bmp"),
			gettexture("res/Terrain_grass_12.bmp"),
			gettexture("res/terrain_alpha.bmp"),
			lnode_trans{float3{}, float3{ 0.f, 0.f, 0.f }, float3{5.f, 5.f, 5.f}},
			},

			// 天空背景
		gettexture("res/sky.bmp"),

		// 场景灯光构造
		lscene_light{ 
			lcolor::white(),
			 std::vector <ldir_light>{ { float3{ 0.f, -2.f, -1.f }, lcolor::white(), 1.f } },
			{},
			.3f, .4f, .3f
		},
		
		// 场景摄像机构造
		lcamera{ 
			lview_trans{{ -0.f, 6.f, -80.f },{ 0.f, 1.f, 0.f },{ 0.f, 0.f, 1.f } },
			lproj_trans{ 3.f / 4.f, L_PIDIV4, 1.f, 1000.f, 800, 600 }
		},

		lcolor_buffer{800, 600},
		ldepth_buffer{800, 600}
	};

	g_scene->init();
}

void Destroy()
{
	if (g_scene)
	{
		delete g_scene;
		g_scene = nullptr;
	}
}

float3 g_objrot{ -0.f, 0.f, 0.f };

lmesh<stVPosNorUV> g_rectmesh{ { stVPosNorUV{ float3{0,0,0},float3{-1,0,0},float2{0,1}}, // 第一个点
	stVPosNorUV{ float3{ 20.f,0,0 },float3{ -1,0,0 },float2{ 1.f,1 } }, // 右边点
	stVPosNorUV{ float3{ 20.f,20.f,0 },float3{ -1,0,0 },float2{ 1.f,0 } },
	stVPosNorUV{ float3{ 0,20.f,0 },float3{ -1,0,0 },float2{ 0,0 } } },
	{ {0,1,2}, {0, 2, 3} } };


void Render(HWND hwnd)
{
	//
	Frame();

	if (g_scene)
	{
		static CTimer rendertimer;
		static int framecount = 0;
		static double tt = 0;
		rendertimer.Start();

		//-----------渲染
		g_scene->render();
		//------渲染结束

		// 计算fps
		tt += rendertimer.Stop();
		framecount += 1;
		static int fps = 100;
		if (tt > 1.0)
		{
			tt = 0;
			fps = framecount;
			framecount = 0;
		}

		// 输出到窗口
		ShowPic(g_scene->clbuf.pbuf.data(), g_scene->clbuf.w, g_scene->clbuf.h, hwnd);
		
		// 显示fps
		HDC hdc = GetDC(hwnd);
		std::stringstream ss;
		ss <<"  FPS:"<< fps << "  三角形总数："<<g_scene->sta.ntri_total << "  背面剔除："<<g_scene->sta.ntri_back_cull
			<< "  视域剔除："<<g_scene->sta.ntri_out_cull<<"  顶点数："<<g_scene->sta.nver << "\n" 
			<< "  像素数："<<g_scene->sta.npixel<<"  深度测试："<<g_scene->sta.npixel_depth_passed 
			<<"  点光源数："<<g_scene->lit_in_view.point_lts.size()<<"\n"
			<<"  -------李连俊 lgraph项目--地形展示 地形贴图：4";
		RECT rect{ 0, 0, 800, 600 };
		SetTextColor(hdc, RGB(100, 100, 100));
		SetBkMode(hdc, TRANSPARENT);
		int sc = ::DrawTextA(hdc, ss.str().c_str(), ss.str().size(), &rect, 0 );
		ReleaseDC(hwnd, hdc);
	}

	return;
}

#define nKey 256
//#define DOWN(key) ( (key & 0x8000) != 0 )

bool DOWN(int key)
{
	return (::GetAsyncKeyState(key) & 0x8000) != 0;
}

void Frame()
{
	// 所有按键的状态
	static int keyStates[255];
	// 鼠标的位置、移动、滚轮
	static int mouseX, mouseY, mouseWheel;

	static CTimer timer;
	static float move_speed = 3.f;
	static float rot_speed = 0.005f;

	auto t = timer.Frame();
	// 获取按键信息
	//for (int i = 0; i < 255; i++)
	//{
	//	keyStates[i] = ::GetAsyncKeyState(i);
	//}
	// 获取鼠标位置
	POINT pos;
	GetCursorPos(&pos);
	mouseX = pos.x; mouseY = pos.y;

	bool bupdate = false;

	if (DOWN(VK_LCONTROL) || DOWN(VK_RCONTROL))
	{
		if (DOWN(VK_LEFT))
		{
			g_rot.y += rot_speed;
			bupdate = true;
		}

		else if (DOWN(VK_RIGHT))
		{
			g_rot.y -= rot_speed;
			bupdate = true;
		}

		else if (DOWN(VK_UP))
		{
			g_rot.x += rot_speed;
			bupdate = true;
		}

		else if (DOWN(VK_DOWN))
		{
			g_rot.x -= rot_speed;
			bupdate = true;
		}

		else if (DOWN(VK_PRIOR))
		{
			g_rot.z += rot_speed;
			bupdate = true;
		}

		else if (DOWN(VK_NEXT))
		{
			g_rot.z -= rot_speed;
			bupdate = true;
		}
	}
	else
	{
		if (DOWN(VK_LEFT))
		{
			g_scene->cam.view_trans.pos.x -= t * move_speed;
			bupdate = true;
		}

		else if (DOWN(VK_RIGHT))
		{
			g_scene->cam.view_trans.pos.x += t * move_speed;
			bupdate = true;
		}

		else if (DOWN(VK_UP))
		{
			g_scene->cam.view_trans.pos.z += t * move_speed;
			bupdate = true;
		}

		else if (DOWN(VK_DOWN))
		{
			g_scene->cam.view_trans.pos.z -= t * move_speed;
			bupdate = true;
		}

		else if (DOWN(VK_PRIOR))
		{
			g_scene->cam.view_trans.pos.y += t * move_speed;
			bupdate = true;
		}

		else if (DOWN(VK_NEXT))
		{
			g_scene->cam.view_trans.pos.y -= t * move_speed;
			bupdate = true;
		}
		else if (DOWN(VK_OEM_PLUS))
		{
			g_scl *= (1.1f);
		}
		else if (DOWN(VK_OEM_MINUS))
		{
			g_scl *= (.9f);
		}
	}
}

LRESULT CALLBACK __WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg) {
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_PAINT:
		//graph.Present();
		//SurfaceToWnd(&g_surface, hWnd);
		//graph.UpdateToWnd();
		if (g_texcat1.buffer.size() > 0)
		{
			//ShowPic(g_texture.buffer.data(), g_texture.w, g_texture.h, hWnd);
		}
		break;

	case WM_MOUSEMOVE:
	{
		if (drawMode == EDrawMode::ePoint)
		{
			if (wParam == MK_LBUTTON)
			{
				POINT pos;
				pos.x = LOWORD(lParam);
				pos.y = HIWORD(lParam);
				graph.DrawWndPoint(pos, g_cl);
				graph.UpdateToWnd();

				//graph.DrawPointWnd(pos, g_cl, g_brush);
				//UpdateWindow(hWnd);
				//graph.Present();
		
			}
		}
		break;
	}


	case WM_LBUTTONDOWN:
		if (drawMode == EDrawMode::eLine || 
			drawMode == EDrawMode::eTrianlge ||
			drawMode == EDrawMode::eCircle ||
			drawMode == EDrawMode::eRect)
		{
			POINT pos;
			pos.x = LOWORD(lParam);
			pos.y = HIWORD(lParam);
			g_points.push(pos);
		}
		
		

		break;

	case WM_LBUTTONUP:
		if (drawMode == EDrawMode::eLine)
		{
			if (g_points.size() > 0)
			{

				POINT pos;
				pos.x = LOWORD(lParam);
				pos.y = HIWORD(lParam);
				POINT last = g_points.top();
				g_points.pop();
				//graph.DrawLineWnd(last, pos, g_cl, g_lineWidth);
				//graph.Present();
				//graph.DrawWndLine(last, pos, g_cl);
				graph.DrawWndLine(last, randcl(), pos, randcl());
				graph.UpdateToWnd();

			}
		}
		else if (drawMode == eRect)
		{
			if (g_points.size() > 0)
			{

				POINT pos;
				pos.x = LOWORD(lParam);
				pos.y = HIWORD(lParam);
				POINT last = g_points.top();
				g_points.pop();
				RECT rect;
				rect.left = min(pos.x, last.x);
				rect.top = min(pos.y, last.y);
				rect.right = max(pos.x, last.x);
				rect.bottom = max(pos.y, last.y);
				g_timer.Start();
				if (g_fill)
				{
					graph.DrawWndRect(rect,randcl(),randcl(),randcl(),randcl());
				}
				else
				{
					graph.DrawWndRectL(rect,randcl());
				}
				std::cout << "画矩形用时" << g_timer.Stop() << std::endl;
				graph.UpdateToWnd();
				
			}

		}
		else if (drawMode == eTrianlge)
		{
			if (g_points.size() == 3)
			{
				ltri2di tri;
				POINT p1 = g_points.top();
				g_points.pop();
				POINT p2 = g_points.top();
				g_points.pop();
				POINT p3 = g_points.top();
				g_points.pop();
				//graph.DrawTriangleWnd(p1, p2, p3,largb::blue(),largb::red(), g_lineWidth );
				//graph.Present();
				//graph.DrawWndTri(p1, p2, p3, g_cl, g_cl, true);
				
				if (g_fill)
				{
					
					graph.DrawWndTri(p1,randcl(), p2,randcl(), p3,randcl());
				}
				else
				{
					graph.DrawWndTriL(p1, p2, p3,randcl());
				}
				
				graph.UpdateToWnd();
			}
		}

		else if (drawMode == eCircle)
		{
			if (g_points.size() > 0)
			{

				POINT pos;
				pos.x = LOWORD(lParam);
				pos.y = HIWORD(lParam);
				POINT last = g_points.top();
				g_points.pop();
				g_timer.Start();
				if (g_fill)
				{
					graph.DrawWndCircle(last, pos,randcl(),randcl());
				}
				else
				{
					graph.DrawWndCircleL(last, pos,randcl());
				}
				std::cout << "画圆用时" << g_timer.Stop() << std::endl;
				//graph.DrawWndLine(last, largb::red(), pos, largb::green());
				graph.UpdateToWnd();

			}
		}
		break;

	case WM_KEYDOWN:
		switch (wParam) {
		case VK_ESCAPE:     //按Esc键退出程序
			PostQuitMessage(0);
			break;
		case 76:
			drawMode = EDrawMode::eLine;
			while (!g_points.empty()) g_points.pop();
			std::cout << "切换到画线模式" << std::endl;
			break;
		case 80:
			drawMode = EDrawMode::ePoint;
			bLineBeg = false;
			std::cout << "切换到画点模式" << std::endl;
			break;
		case 81:
			// 输入画线
		{
			lline2di line;
			std::cout << "输入线的两个点:" << std::endl;
			std::cin >> line.p1.x >> line.p1.y >> line.p2.x >> line.p2.y;
			graph.DrawBSLine(line, g_cl);
				graph.UpdateToWnd();
			break;
		}
		case VK_OEM_4:
			g_lineWidth--;
			g_lineWidth = g_lineWidth < 0 ? 0 : g_lineWidth;
			std::cout << "设置线宽为" << g_lineWidth << std::endl;
			break;
		case VK_OEM_6:
			g_lineWidth++;
			std::cout << "设置线宽为" << g_lineWidth << std::endl;
			break;
			// C 切换颜色
		case 67:
			g_cl =randcl();
			std::cout << "切换随机颜色" << std::endl;
			break;
			// T 三角形
		case 84:
			drawMode = EDrawMode::eTrianlge;
			while( !g_points.empty()) g_points.pop();
			std::cout << "切换到画三角形模式" << std::endl;
			break;

			// Y 画圆
		case 88:
			drawMode = EDrawMode::eCircle;
			while (!g_points.empty()) g_points.pop();
			std::cout << "切换到画圆模式" << std::endl;
			break;

			// F：切换填充
		case 70:
			g_fill = g_fill ? (std::cout<<"关闭填充\n",false) : (std::cout<<"开启填充\n", true);
			break;

			// R:画矩形
		case 82:
			drawMode = EDrawMode::eRect;
			while (!g_points.empty()) g_points.pop();
			std::cout << "切换到矩形模式" << std::endl;
			break;
		}
		break;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}