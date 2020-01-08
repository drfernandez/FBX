#pragma once

// includes
#include "header.h"

#define SAFE_RELEASE(ptr) { if(ptr) { ptr->Release(); ptr = nullptr; } }

class InputLayoutManager
{
public:
	enum IL_TYPE { PCTNTBJW = 0, COUNT };

private:
	// proper singleton closure
	InputLayoutManager() = default;
	~InputLayoutManager() = default;
	InputLayoutManager(const InputLayoutManager& copy) = default;
	InputLayoutManager& operator=(const InputLayoutManager& copy) = default;

	// data members
	ID3D11Device*				m_Device = nullptr;
	ID3D11InputLayout*			m_Layouts[IL_TYPE::COUNT] = { NULL };

	// private functions
	BOOL ReadCSOFromFile(const char* pFileName, char** pByteCode, size_t& nByteCodeSize);
	void AddInputLayout(IL_TYPE type, const char* pFilename);


public:
	// singleton access
	static InputLayoutManager* GetInstance(void);

	// public functions
	void Initialize(ID3D11Device* pDevice);
	void Shutdown(void);

	ID3D11InputLayout* GetInputLayout(IL_TYPE type);

};
