#include "Mesh.h"

Mesh::Mesh()
{
	m_MeshName = "";
	m_Vertices.clear();
	m_VertOffset = -1;
	m_VertTotal = -1;
	m_Indicies.clear();
	m_IndexOffset = -1;
	m_IndexTotal = -1;
	m_WorldMatrix = DirectX::XMMatrixIdentity();
	m_TextureDiffuseID = -1;
	m_TextureNormalID = -1;
	m_TextureSpecularID = -1;
	m_TextureDiffuseName = "";
	m_TextureNormalName = "";
	m_TextureSpecularName = "";
}

Mesh::~Mesh()
{
	m_MeshName = "";
	m_Vertices.clear();
	m_VertOffset = -1;
	m_VertTotal = -1;
	m_Indicies.clear();
	m_IndexOffset = -1;
	m_IndexTotal = -1;
	m_WorldMatrix = DirectX::XMMatrixIdentity();
	m_TextureDiffuseID = -1;
	m_TextureNormalID = -1;
	m_TextureSpecularID = -1;
	m_TextureDiffuseName = "";
	m_TextureNormalName = "";
	m_TextureSpecularName = "";
}

Mesh::Mesh(const Mesh& copy)
{
	m_MeshName = copy.m_MeshName;
	m_Vertices = copy.m_Vertices;
	m_VertOffset = copy.m_VertOffset;
	m_VertTotal = copy.m_VertTotal;
	m_Indicies = copy.m_Indicies;
	m_IndexOffset = copy.m_IndexOffset;
	m_IndexTotal = copy.m_IndexTotal;
	m_WorldMatrix = copy.m_WorldMatrix;
	m_TextureDiffuseID = copy.m_TextureDiffuseID;
	m_TextureNormalID = copy.m_TextureNormalID;
	m_TextureSpecularID = copy.m_TextureSpecularID;
	m_TextureDiffuseName = copy.m_TextureDiffuseName;
	m_TextureNormalName = copy.m_TextureNormalName;
	m_TextureSpecularName = copy.m_TextureSpecularName;
}

Mesh& Mesh::operator=(const Mesh& copy)
{
	if (this != &copy)
	{
		m_MeshName = copy.m_MeshName;
		m_Vertices = copy.m_Vertices;
		m_VertOffset = copy.m_VertOffset;
		m_VertTotal = copy.m_VertTotal;
		m_Indicies = copy.m_Indicies;
		m_IndexOffset = copy.m_IndexOffset;
		m_IndexTotal = copy.m_IndexTotal;
		m_WorldMatrix = copy.m_WorldMatrix;
		m_TextureDiffuseID = copy.m_TextureDiffuseID;
		m_TextureNormalID = copy.m_TextureNormalID;
		m_TextureSpecularID = copy.m_TextureSpecularID;
		m_TextureDiffuseName = copy.m_TextureDiffuseName;
		m_TextureNormalName = copy.m_TextureNormalName;
		m_TextureSpecularName = copy.m_TextureSpecularName;
	}
	return *this;
}