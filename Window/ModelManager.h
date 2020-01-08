#pragma once

#include "header.h"
#include "BinaryReader.h"
#include "DynamicModel.h"
#include "StaticModel.h"

#define SAFE_DELETE(ptr) { if(ptr) { delete ptr; ptr = nullptr; } }

class ModelManager
{
private:
	// proper singleton closure
	ModelManager() = default;
	~ModelManager() = default;
	ModelManager(const ModelManager& copy) = default;
	ModelManager& operator=(const ModelManager& copy) = default;

	// data members
	BinaryReader*						m_pBinaryReader;
	VertexBufferManager*				m_pVertBufferManager;
	IndexBufferManager*					m_pIndexBufferManager;
	TextureManager*						m_pTextureManager;

	std::vector<DynamicModel*>			m_vDynamicModels;
	std::vector<StaticModel*>			m_vStaticModels;

public:
	// singleton access
	static ModelManager* GetInstance(void);

	// public functions
	void Initialize(BinaryReader* br, VertexBufferManager* vb, IndexBufferManager* ib, TextureManager* tm);
	void Shutdown(void);

	void AddDynamicModel(std::string name, std::string mesh, std::string pose, std::vector<std::string> animations);
	void AddStaticModel(std::string name, std::string mesh);

	inline std::vector<DynamicModel*>& GetDynamicModels(void) { return m_vDynamicModels; }
	inline const std::vector<DynamicModel*>& GetDynamicModels(void) const { return m_vDynamicModels; }
	inline std::vector<StaticModel*>& GetStaticModels(void) { return m_vStaticModels; }
	inline const std::vector<StaticModel*>& GetStaticModels(void) const { return m_vStaticModels; }

	DynamicModel* GetDynamicModel(unsigned int index);
	DynamicModel* GetDynamicModel(std::string name);
	StaticModel* GetStaticModel(unsigned int index);
	StaticModel* GetStaticModel(std::string name);
};