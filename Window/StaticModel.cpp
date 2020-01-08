#include "StaticModel.h"

bool StaticModel::LoadMesh(const std::string & pFilename, std::vector<Mesh*>& mesh)
{
	BOOL success = false;
	success = m_BinaryReader->ReadMeshFromFile(pFilename.c_str(), mesh);

	if (!success) return FALSE;

	for (int i = 0; i < (int)mesh.size(); i++)
	{
		mesh[i]->m_VertOffset = m_VertBufferManager->GetOffsets()[i];
		mesh[i]->m_VertTotal = (INT)mesh[i]->m_Vertices.size();
		m_VertBufferManager->AddDataToBuffer(mesh[i]->m_Vertices, m_VertBufferManager->GetOffsets());
		mesh[i]->m_IndexOffset = m_IndexBufferManager->GetOffsets()[IndexBufferManager::IB_TYPE::UINT32];
		mesh[i]->m_IndexTotal = (INT)mesh[i]->m_Indicies.size();
		m_IndexBufferManager->AddDataToBuffer(mesh[i]->m_Indicies, m_IndexBufferManager->GetOffsets());

		mesh[i]->m_WorldMatrix = DirectX::XMMatrixIdentity();

		std::string prefix = "../Assets/";
		std::string concat = "";
		concat = prefix + mesh[i]->m_TextureDiffuseName;
		mesh[i]->m_TextureDiffuseID = m_TextureManager->AddTexture(concat.c_str(), TextureManager::TM_TYPE::DIFFUSE);
		concat = prefix + mesh[i]->m_TextureNormalName;
		mesh[i]->m_TextureNormalID = m_TextureManager->AddTexture(concat.c_str(), TextureManager::TM_TYPE::NORMAL);
		concat = prefix + mesh[i]->m_TextureSpecularName;
		mesh[i]->m_TextureSpecularID = m_TextureManager->AddTexture(concat.c_str(), TextureManager::TM_TYPE::SPECULAR);
		
		mesh[i]->m_WorldMatrix = DirectX::XMMatrixTranslation(0.0f, 0.0f, 0.0f);

		//mesh[i]->m_Vertices.clear();
		//mesh[i]->m_Indicies.clear();
	}

	return TRUE;
}

StaticModel::StaticModel()
{
	m_BinaryReader = nullptr;
	m_VertBufferManager = nullptr;
	m_IndexBufferManager = nullptr;
	m_TextureManager = nullptr;
	m_Name = "";
	m_Meshes.clear();
}

StaticModel::StaticModel(BinaryReader * reader, VertexBufferManager * vbm, IndexBufferManager * ibm, TextureManager * tm)
{
	m_BinaryReader = reader;
	m_VertBufferManager = vbm;
	m_IndexBufferManager = ibm;
	m_TextureManager = tm;
	m_Name = "";
	m_Meshes.clear();
}

StaticModel::~StaticModel()
{
	m_BinaryReader = nullptr;
	m_VertBufferManager = nullptr;
	m_IndexBufferManager = nullptr;
	m_TextureManager = nullptr;
	m_Name = "";
	for (size_t i = 0; i < m_Meshes.size(); i++)
	{
		SAFE_DELETE(m_Meshes[i]);
	}
	m_Meshes.clear();
}

//StaticModel::StaticModel(const StaticModel & copy)
//{
//	m_BinaryReader = copy.m_BinaryReader;
//	m_VertBufferManager = copy.m_VertBufferManager;
//	m_IndexBufferManager = copy.m_IndexBufferManager;
//	m_TextureManager = copy.m_TextureManager;
//	m_Name = copy.m_Name;
//	for (size_t i = 0; i < copy.m_Meshes.size(); i++)
//	{
//		Mesh* temp_mesh = new Mesh();
//		memcpy_s(temp_mesh, sizeof(copy.m_Meshes[i]), copy.m_Meshes[i], sizeof(copy.m_Meshes[i]));
//		m_Meshes.push_back(temp_mesh);
//	}
//}
//
//StaticModel & StaticModel::operator=(const StaticModel & copy)
//{
//	if (this != &copy)
//	{
//		m_BinaryReader = copy.m_BinaryReader;
//		m_VertBufferManager = copy.m_VertBufferManager;
//		m_IndexBufferManager = copy.m_IndexBufferManager;
//		m_TextureManager = copy.m_TextureManager;
//		m_Name = copy.m_Name;
//		for (size_t i = 0; i < copy.m_Meshes.size(); i++)
//		{
//			Mesh* temp_mesh = new Mesh();
//			memcpy_s(temp_mesh, sizeof(copy.m_Meshes[i]), copy.m_Meshes[i], sizeof(copy.m_Meshes[i]));
//			m_Meshes.push_back(temp_mesh);
//		}
//	}
//	return *this;
//}

void StaticModel::LoadModelFromFile(const std::string & szMeshFilename)
{
	LoadMesh(szMeshFilename, m_Meshes);
}

DirectX::XMMATRIX& StaticModel::GetWorldMatrix(void)
{
	return m_Meshes[0]->m_WorldMatrix;
}

const DirectX::XMMATRIX & StaticModel::GetWorldMatrix(void) const
{
	return m_Meshes[0]->m_WorldMatrix;
}

void StaticModel::SetWorldMatrix(const DirectX::XMMATRIX & wm)
{
	for (size_t i = 0; i < m_Meshes.size(); i++)
	{
		m_Meshes[i]->m_WorldMatrix = wm;
	}
}

