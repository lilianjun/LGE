#pragma once

#include <fstream>
#include "lmath.hpp"


struct _stLMeshFileHeader
{
	// �����ļ���ͷ
	// ��־�ַ�����Ϊ"lmesh"
	char flag[8];
	// �����������
	char name[64];
	// ��������
	unsigned int numVert;
	// ����������
	unsigned int numTri;
	// ��Ƥ����
	unsigned int numSkin;
	// ����ĸ�ʽ
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
