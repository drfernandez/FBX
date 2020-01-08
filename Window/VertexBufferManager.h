#pragma once

// includes
#include "header.h"

#define SAFE_RELEASE(ptr) { if(ptr) { ptr->Release(); ptr = nullptr; } }
#define MAXSIZE 10000000

class VertexBufferManager
{
public:
	enum VB_TYPE { POSITION = 0, COLOR, TEXCOORD, NORMAL, TANGENT, BINORMAL, JOINTS, WEIGHTS, COUNT };

private:
	// proper singleton closure
	VertexBufferManager() = default;
	~VertexBufferManager() = default;
	VertexBufferManager(const VertexBufferManager& copy) = default;
	VertexBufferManager& operator=(const VertexBufferManager& copy) = default;

	// data Members
	ID3D11Device*					m_Device = nullptr;
	ID3D11DeviceContext*			m_Context = nullptr;
	ID3D11Buffer*					m_Buffers[VB_TYPE::COUNT] = { NULL };
	INT								m_Offsets[VB_TYPE::COUNT] = { NULL };
	BOOL							m_FirstUpdate = true;

	// private functions
	HRESULT InitializeBuffer(VB_TYPE type, UINT sizeInBytes);


public:
	// singleton access
	static VertexBufferManager* GetInstance(void);

	// public functions
	void Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	void Shutdown(void);

	void AddDataToBuffer(const std::vector<Vertex>& data, INT* offset);

	ID3D11Buffer* GetVertexBuffer(VB_TYPE type);
	INT* GetOffsets(void);

};

