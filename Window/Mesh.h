#pragma once

#include <vector>
#include <DirectXMath.h>
#include "Vertex.h"

struct Mesh
{
public:
	std::string					m_MeshName;
	std::vector<Vertex>			m_Vertices;
	int							m_VertOffset;
	int							m_VertTotal;
	std::vector<unsigned int>	m_Indicies;
	int							m_IndexOffset;
	int							m_IndexTotal;
	DirectX::XMMATRIX			m_WorldMatrix;
	int							m_TextureDiffuseID;
	int							m_TextureNormalID;
	int							m_TextureSpecularID;
	std::string					m_TextureDiffuseName;
	std::string					m_TextureNormalName;
	std::string					m_TextureSpecularName;


public:
	// Constructor / Destructor / Copy Constructor / Assignment Operator
	Mesh();
	~Mesh();
	Mesh(const Mesh& copy);
	Mesh& operator=(const Mesh& copy);
};