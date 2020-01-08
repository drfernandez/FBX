#pragma once

// includes
#include "header.h"

#define SAFE_RELEASE(ptr) { if(ptr) { ptr->Release(); ptr = nullptr; } }
#define MAXSIZE 10000000

class IndexBufferManager
{
public:
	enum IB_TYPE { UINT32 = 0, COUNT };

private:
	// proper singleton closure
	IndexBufferManager() = default;
	~IndexBufferManager() = default;
	IndexBufferManager(const IndexBufferManager& copy) = default;
	IndexBufferManager& operator=(const IndexBufferManager& copy) = default;

	// data members
	ID3D11Device*					m_Device = nullptr;
	ID3D11DeviceContext*			m_Context = nullptr;
	ID3D11Buffer*					m_Buffers[IB_TYPE::COUNT] = { NULL };
	INT								m_Offsets[IB_TYPE::COUNT] = { NULL };
	BOOL							m_FirstUpdate = true;

	// private functions
	HRESULT InitializeBuffer(IB_TYPE type, UINT sizeInBytes);

public:
	// singleton access
	static IndexBufferManager* GetInstance(void);

	// public functions
	void Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	void Shutdown(void);

	void AddDataToBuffer(const std::vector<UINT>& data, INT* offset);

	ID3D11Buffer* GetIndexBuffer(IB_TYPE type);
	INT* GetOffsets(void);
};