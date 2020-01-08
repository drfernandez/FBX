#include "IndexBufferManager.h"

HRESULT IndexBufferManager::InitializeBuffer(IB_TYPE type, UINT sizeInBytes)
{
	HRESULT hr = 0;
	BYTE* buffer = new BYTE[sizeInBytes];

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_INDEX_BUFFER;
	bd.ByteWidth = sizeInBytes;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_WRITE;
	bd.MiscFlags = 0;
	bd.StructureByteStride = 0;
	bd.Usage = D3D11_USAGE::D3D11_USAGE_DYNAMIC;

	D3D11_SUBRESOURCE_DATA srd;
	ZeroMemory(&srd, sizeof(srd));
	srd.pSysMem = buffer;
	srd.SysMemPitch = 0;
	srd.SysMemSlicePitch = 0;

	hr = m_Device->CreateBuffer(&bd, &srd, &m_Buffers[type]);


	delete[] buffer;
	return hr;
}

IndexBufferManager * IndexBufferManager::GetInstance(void)
{
	static IndexBufferManager instance;
	return &instance;
}

void IndexBufferManager::Initialize(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	m_Device = pDevice;
	m_Context = pContext;

	InitializeBuffer(IB_TYPE::UINT32, sizeof(UINT) * MAXSIZE);
}

void IndexBufferManager::Shutdown(void)
{
	for (size_t i = 0; i < IB_TYPE::COUNT; i++)
	{
		SAFE_RELEASE(m_Buffers[i]);
	}
	m_Context = nullptr;
	m_Device = nullptr;
}

void IndexBufferManager::AddDataToBuffer(const std::vector<UINT>& data, INT * offset)
{
	if (sizeof(UINT) * data.size() + offset[0] > sizeof(UINT) * MAXSIZE) return;

	UINT* ptr = nullptr;

	D3D11_MAPPED_SUBRESOURCE msr;
	if (m_FirstUpdate)
	{
		m_Context->Map(m_Buffers[IB_TYPE::UINT32], 0, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, 0, &msr);
		ptr = (UINT*)msr.pData;
		memcpy_s(&ptr[offset[IB_TYPE::UINT32]], sizeof(UINT) * MAXSIZE, data.data(), sizeof(UINT) * data.size());
		m_Context->Unmap(m_Buffers[IB_TYPE::UINT32], 0);
		m_FirstUpdate = false;
	}
	else
	{
		m_Context->Map(m_Buffers[IB_TYPE::UINT32], 0, D3D11_MAP::D3D11_MAP_WRITE_NO_OVERWRITE, 0, &msr);
		ptr = (UINT*)msr.pData;
		memcpy_s(&ptr[offset[IB_TYPE::UINT32]], sizeof(UINT) * MAXSIZE, data.data(), sizeof(UINT) * data.size());
		m_Context->Unmap(m_Buffers[IB_TYPE::UINT32], 0);
	}

	m_Offsets[IB_TYPE::UINT32] += (INT)data.size();
}

ID3D11Buffer * IndexBufferManager::GetIndexBuffer(IB_TYPE type)
{
	return m_Buffers[type];
}

INT * IndexBufferManager::GetOffsets(void)
{
	return m_Offsets;
}
