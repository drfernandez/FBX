#pragma once

// includes
#include "header.h"

#define SAFE_RELEASE(ptr) { if(ptr) { ptr->Release(); ptr = nullptr; } }

class VertexShaderManager
{
public:
	enum VS_TYPE { PCTNTBJW = 0, PCTNTBJW_ANIM, SKYBOX, DEBUGLINES, COUNT };

private:
	// proper singleton closure
	VertexShaderManager() = default;
	~VertexShaderManager() = default;
	VertexShaderManager(const VertexShaderManager& copy) = default;
	VertexShaderManager& operator=(const VertexShaderManager& copy) = default;

	// data Members
	ID3D11Device*					m_Device = nullptr;
	ID3D11VertexShader*				m_Shaders[VS_TYPE::COUNT] = { NULL };

	// private functions
	BOOL ReadCSOFromFile(const char* pFileName, char** pByteCode, size_t& nByteCodeSize);
	void AddVertexShader(VS_TYPE type, const char* pFilename);

public:
	// singleton access
	static VertexShaderManager* GetInstance(void);

	// public functions
	void Initialize(ID3D11Device* pDevice);
	void Shutdown(void);

	ID3D11VertexShader* GetVertexShader(VS_TYPE type);
};

