#pragma once

// includes
#include "header.h"

#define SAFE_RELEASE(ptr) { if(ptr) { ptr->Release(); ptr = nullptr; } }

class TextureManager
{
public:
	enum TM_TYPE { DIFFUSE = 0, NORMAL, SPECULAR, COUNT };

private:
	// proper singleton closure
	TextureManager() = default;
	~TextureManager() = default;
	TextureManager(const TextureManager& copy) = default;
	TextureManager& operator=(const TextureManager& copy) = default;

	// data members
	ID3D11Device*											m_Device = nullptr;
	ID3D11DeviceContext*									m_Context = nullptr;
	std::vector<ID3D11ShaderResourceView*>					m_Diffuse;
	std::unordered_map<std::string, UINT>					m_DiffuseSearchMap;
	std::unordered_map<std::string, UINT>::iterator			m_DiffuseSearchMapIter;
	std::vector<ID3D11ShaderResourceView*>					m_Normal;
	std::unordered_map<std::string, UINT>					m_NormalSearchMap;
	std::unordered_map<std::string, UINT>::iterator			m_NormalSearchMapIter;
	std::vector<ID3D11ShaderResourceView*>					m_Specular;
	std::unordered_map<std::string, UINT>					m_SpecularSearchMap;
	std::unordered_map<std::string, UINT>::iterator			m_SpecularSearchMapIter;

	// private functions
	std::wstring to_wstring(std::string const& str);
	int ParseFileExtension(const std::string& filename);

public:
	// singleton access
	static TextureManager* GetInstance(void);

	// public functions
	void Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	void Shutdown(void);

	INT AddTexture(const std::string& pFilename, TM_TYPE type);
	ID3D11ShaderResourceView* GetShaderResourceView(TM_TYPE type, INT id);
};