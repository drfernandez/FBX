#pragma once

// includes
#include "header.h"

#define SAFE_RELEASE(ptr) { if(ptr) { ptr->Release(); ptr = nullptr; } }

class PixelShaderManager
{
public:
	enum PS_TYPE { PCTNTBJW = 0, PCTNTBJW_TEXONLY, SKYBOX, DEBUGLINES, COUNT };

private:
	// proper singleton closure
	PixelShaderManager() = default;
	~PixelShaderManager() = default;
	PixelShaderManager(const PixelShaderManager& copy) = default;
	PixelShaderManager& operator=(const PixelShaderManager& copy) = default;

	// data Members
	ID3D11Device*					m_Device = nullptr;
	ID3D11PixelShader*				m_Shaders[PS_TYPE::COUNT] = { NULL };

	// private functions
	BOOL ReadCSOFromFile(const char* pFileName, char** pByteCode, size_t& nByteCodeSize);
	void AddPixelShader(PS_TYPE type, const char* pFilename);

public:
	// singleton access
	static PixelShaderManager* GetInstance(void);

	// public functions
	void Initialize(ID3D11Device* pDevice);
	void Shutdown(void);

	ID3D11PixelShader* GetPixelShader(PS_TYPE type);
};