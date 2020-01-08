#pragma once

#include "Structures.h"
#include "BinaryReader.h"
#include "VertexBufferManager.h"
#include "IndexBufferManager.h"
#include "TextureManager.h"

class StaticModel
{
protected:
	BinaryReader*				m_BinaryReader;
	VertexBufferManager*		m_VertBufferManager;
	IndexBufferManager*			m_IndexBufferManager;
	TextureManager*				m_TextureManager;

	std::string					m_Name;
	std::vector<Mesh*>			m_Meshes;

	bool LoadMesh(const std::string& pFilename, std::vector<Mesh*>& mesh);

public:
	StaticModel();
	StaticModel(BinaryReader* reader, VertexBufferManager* vbm, IndexBufferManager* ibm, TextureManager* tm);
	~StaticModel();
	StaticModel(const StaticModel& copy) = delete;
	StaticModel& operator=(const StaticModel& copy) = delete;


	virtual void LoadModelFromFile(const std::string& szMeshFilename);
	inline void SetMeshes(const std::vector<Mesh*>& meshes) { m_Meshes = meshes; }

	inline std::vector<Mesh*>& GetMeshes(void) { return m_Meshes; }
	inline const std::vector<Mesh*>& GetMeshes(void) const { return m_Meshes; }

	inline std::string& GetName(void) { return m_Name; }
	inline const std::string& GetName(void) const { return m_Name; }
	inline void SetName(std::string n) { m_Name = n; }

	DirectX::XMMATRIX& GetWorldMatrix(void);
	const DirectX::XMMATRIX& GetWorldMatrix(void) const;
	void SetWorldMatrix(const DirectX::XMMATRIX& wm);
};