#include "ModelManager.h"

ModelManager * ModelManager::GetInstance(void)
{
	static ModelManager instance;
	return &instance;
}

void ModelManager::Initialize(BinaryReader* br, VertexBufferManager* vb, IndexBufferManager* ib, TextureManager* tm)
{
	m_pBinaryReader = br;
	m_pVertBufferManager = vb;
	m_pIndexBufferManager = ib;
	m_pTextureManager = tm;

	m_vDynamicModels.clear();
	m_vStaticModels.clear();
}

void ModelManager::Shutdown(void)
{
	m_pBinaryReader = nullptr;
	m_pVertBufferManager = nullptr;
	m_pIndexBufferManager = nullptr;
	m_pTextureManager = nullptr;

	for (size_t i = 0; i < m_vDynamicModels.size(); i++)
	{
		SAFE_DELETE(m_vDynamicModels[i]);
	}
	m_vDynamicModels.clear();
	for (size_t i = 0; i < m_vStaticModels.size(); i++)
	{
		SAFE_DELETE(m_vStaticModels[i]);
	}
	m_vStaticModels.clear();
}

void ModelManager::AddDynamicModel(std::string name, std::string mesh, std::string pose, std::vector<std::string> animations)
{
	DynamicModel* dynamic_model = new DynamicModel(m_pBinaryReader, m_pVertBufferManager, m_pIndexBufferManager, m_pTextureManager);
	dynamic_model->SetName(name);
	dynamic_model->LoadModelFromFile(mesh, pose, animations);
	m_vDynamicModels.push_back(dynamic_model);
}

void ModelManager::AddStaticModel(std::string name, std::string mesh)
{
	StaticModel* static_model = new StaticModel(m_pBinaryReader, m_pVertBufferManager, m_pIndexBufferManager, m_pTextureManager);
	static_model->SetName(name);
	static_model->LoadModelFromFile(mesh);
	m_vStaticModels.push_back(static_model);
}

DynamicModel * ModelManager::GetDynamicModel(unsigned int index)
{
	DynamicModel* return_value = nullptr;
	if (index < m_vDynamicModels.size()) 
	{
		return_value = m_vDynamicModels[index];
	}
	return return_value;
}

DynamicModel * ModelManager::GetDynamicModel(std::string name)
{
	DynamicModel* return_value = nullptr;
	for (size_t i = 0; i < m_vDynamicModels.size(); i++)
	{
		if (name == m_vDynamicModels[(int)i]->GetName())
		{
			return_value = m_vDynamicModels[(int)i];
			break;
		}
	}
	return return_value;
}

StaticModel * ModelManager::GetStaticModel(unsigned int index)
{
	StaticModel* return_value = nullptr;
	if (index < m_vStaticModels.size())
	{
		return_value = m_vStaticModels[index];
	}
	return return_value;
}

StaticModel * ModelManager::GetStaticModel(std::string name)
{
	StaticModel* return_value = nullptr;
	for (size_t i = 0; i < m_vStaticModels.size(); i++)
	{
		if (name == m_vStaticModels[(int)i]->GetName())
		{
			return_value = m_vStaticModels[(int)i];
			break;
		}
	}
	return return_value;
}
