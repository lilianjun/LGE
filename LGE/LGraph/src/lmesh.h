#pragma once

#include <fstream>
#include "lmath.hpp"


struct _stLMeshFileHeader
{
	// 网格文件的头
	// 标志字符串，为"lmesh"
	char flag[8];
	// 该网格的名字
	char name[64];
	// 顶点数量
	unsigned int numVert;
	// 三角形数量
	unsigned int numTri;
	// 蒙皮数量
	unsigned int numSkin;
	// 顶点的格式
	int vertFmt;
};

template <size_t id>
struct lvertex
{
	float3 pos;
};


template <typename t_vertex>
struct lmesh
{
	std::vector<t_vertex> verts;
	std::vector<lint3> tris;

	inline bool load(const char* filename)
	{
		std::ifstream inf(filename, std::ifstream::binary);
		if (inf)
		{
			_stLMeshFileHeader header;
			inf.read((char*)&header, sizeof(header));
			if (std::string(header.flag) == "lmesh")
			{
				this->verts.resize(header.numVert);
				inf.read((char*)verts.data(), header.numVert * sizeof(t_vertex));
				this->tris.resize(header.numTri);
				inf.read((char*)tris.data(), header.numTri * sizeof(lint3));
				inf.close();
				return true;
			}
		}
		return false;
	}

	template <typename t_render>
	void render(t_render& rd)
	{
		rd.render(verts.data(), verts.size(), tris.data(), tris.size());
	}

};
