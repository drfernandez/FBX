#include "VertexBufferManager.h"

HRESULT VertexBufferManager::InitializeBuffer(VB_TYPE type, UINT sizeInBytes)
{
	HRESULT hr = 0;
	BYTE* buffer = new BYTE[sizeInBytes];

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_VERTEX_BUFFER;
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

VertexBufferManager * VertexBufferManager::GetInstance(void)
{
	static VertexBufferManager instance;
	return &instance;
}

void VertexBufferManager::Initialize(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	// pointers to device and context
	// DO NOT RELEASE in this class
	m_Device = pDevice;
	m_Context = pContext;

	InitializeBuffer(VB_TYPE::POSITION, sizeof(DirectX::XMFLOAT4) * MAXSIZE);
	InitializeBuffer(VB_TYPE::COLOR,	sizeof(DirectX::XMFLOAT4) * MAXSIZE);
	InitializeBuffer(VB_TYPE::TEXCOORD, sizeof(DirectX::XMFLOAT2) * MAXSIZE);
	InitializeBuffer(VB_TYPE::NORMAL,	sizeof(DirectX::XMFLOAT4) * MAXSIZE);
	InitializeBuffer(VB_TYPE::TANGENT,	sizeof(DirectX::XMFLOAT4) * MAXSIZE);
	InitializeBuffer(VB_TYPE::BINORMAL, sizeof(DirectX::XMFLOAT4) * MAXSIZE);
	InitializeBuffer(VB_TYPE::JOINTS,   sizeof(DirectX::XMFLOAT4) * MAXSIZE);
	InitializeBuffer(VB_TYPE::WEIGHTS,  sizeof(DirectX::XMFLOAT4) * MAXSIZE);

	ZeroMemory(m_Offsets, sizeof(m_Offsets));
}

void VertexBufferManager::Shutdown(void)
{
	for (size_t i = 0; i < VB_TYPE::COUNT; i++)
	{
		SAFE_RELEASE(m_Buffers[i]);
	}

	m_Context = nullptr;
	m_Device = nullptr;
}

void VertexBufferManager::AddDataToBuffer(const std::vector<Vertex>& data, INT* offset)
{
	if (sizeof(DirectX::XMFLOAT4) * data.size() + offset[0] > sizeof(DirectX::XMFLOAT4) * MAXSIZE) return;

	std::vector<DirectX::XMFLOAT4> position;
	std::vector<DirectX::XMFLOAT4> color;
	std::vector<DirectX::XMFLOAT2> texcoord;
	std::vector<DirectX::XMFLOAT4> normal;
	std::vector<DirectX::XMFLOAT4> tangent;
	std::vector<DirectX::XMFLOAT4> binormal;
	std::vector<DirectX::XMFLOAT4> joints;
	std::vector<DirectX::XMFLOAT4> weights;

	for (size_t i = 0; i < data.size(); i++)
	{
		position.push_back(data[i].position);
		color.push_back(data[i].color);
		texcoord.push_back(data[i].texcoord);
		normal.push_back(data[i].normal);
		tangent.push_back(data[i].tangent);
		binormal.push_back(data[i].binormal);
		joints.push_back(data[i].joint);
		weights.push_back(data[i].weight);
	}

	DirectX::XMFLOAT4* ptr4 = nullptr;
	DirectX::XMFLOAT2* ptr2 = nullptr;

	D3D11_MAPPED_SUBRESOURCE msr;
	if (m_FirstUpdate)
	{
		m_Context->Map(m_Buffers[VB_TYPE::POSITION], 0, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, 0, &msr);
		ptr4 = (DirectX::XMFLOAT4*)msr.pData;
		memcpy_s(&ptr4[offset[VB_TYPE::POSITION]], sizeof(DirectX::XMFLOAT4) * MAXSIZE, position.data(), sizeof(DirectX::XMFLOAT4) * position.size());
		m_Context->Unmap(m_Buffers[VB_TYPE::POSITION], 0);

		m_Context->Map(m_Buffers[VB_TYPE::COLOR], 0, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, 0, &msr);
		ptr4 = (DirectX::XMFLOAT4*)msr.pData;
		memcpy_s(&ptr4[offset[VB_TYPE::COLOR]], sizeof(DirectX::XMFLOAT4) * MAXSIZE, color.data(), sizeof(DirectX::XMFLOAT4) * color.size());
		m_Context->Unmap(m_Buffers[VB_TYPE::COLOR], 0);

		m_Context->Map(m_Buffers[VB_TYPE::TEXCOORD], 0, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, 0, &msr);
		ptr2 = (DirectX::XMFLOAT2*)msr.pData;
		memcpy_s(&ptr2[offset[VB_TYPE::TEXCOORD]], sizeof(DirectX::XMFLOAT2) * MAXSIZE, texcoord.data(), sizeof(DirectX::XMFLOAT2) * texcoord.size());
		m_Context->Unmap(m_Buffers[VB_TYPE::TEXCOORD], 0);

		m_Context->Map(m_Buffers[VB_TYPE::NORMAL], 0, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, 0, &msr);
		ptr4 = (DirectX::XMFLOAT4*)msr.pData;
		memcpy_s(&ptr4[offset[VB_TYPE::NORMAL]], sizeof(DirectX::XMFLOAT4) * MAXSIZE, normal.data(), sizeof(DirectX::XMFLOAT4) * normal.size());
		m_Context->Unmap(m_Buffers[VB_TYPE::NORMAL], 0);

		m_Context->Map(m_Buffers[VB_TYPE::TANGENT], 0, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, 0, &msr);
		ptr4 = (DirectX::XMFLOAT4*)msr.pData;
		memcpy_s(&ptr4[offset[VB_TYPE::TANGENT]], sizeof(DirectX::XMFLOAT4) * MAXSIZE, tangent.data(), sizeof(DirectX::XMFLOAT4) * tangent.size());
		m_Context->Unmap(m_Buffers[VB_TYPE::TANGENT], 0);

		m_Context->Map(m_Buffers[VB_TYPE::BINORMAL], 0, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, 0, &msr);
		ptr4 = (DirectX::XMFLOAT4*)msr.pData;
		memcpy_s(&ptr4[offset[VB_TYPE::BINORMAL]], sizeof(DirectX::XMFLOAT4) * MAXSIZE, binormal.data(), sizeof(DirectX::XMFLOAT4) * binormal.size());
		m_Context->Unmap(m_Buffers[VB_TYPE::BINORMAL], 0);

		m_Context->Map(m_Buffers[VB_TYPE::JOINTS], 0, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, 0, &msr);
		ptr4 = (DirectX::XMFLOAT4*)msr.pData;
		memcpy_s(&ptr4[offset[VB_TYPE::JOINTS]], sizeof(DirectX::XMFLOAT4) * MAXSIZE, joints.data(), sizeof(DirectX::XMFLOAT4) * joints.size());
		m_Context->Unmap(m_Buffers[VB_TYPE::JOINTS], 0);

		m_Context->Map(m_Buffers[VB_TYPE::WEIGHTS], 0, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, 0, &msr);
		ptr4 = (DirectX::XMFLOAT4*)msr.pData;
		memcpy_s(&ptr4[offset[VB_TYPE::WEIGHTS]], sizeof(DirectX::XMFLOAT4) * MAXSIZE, weights.data(), sizeof(DirectX::XMFLOAT4) * weights.size());
		m_Context->Unmap(m_Buffers[VB_TYPE::WEIGHTS], 0);

		m_FirstUpdate = false;
	}
	else
	{
		m_Context->Map(m_Buffers[VB_TYPE::POSITION], 0, D3D11_MAP::D3D11_MAP_WRITE_NO_OVERWRITE, 0, &msr);
		ptr4 = (DirectX::XMFLOAT4*)msr.pData;
		memcpy_s(&ptr4[offset[VB_TYPE::POSITION]], sizeof(DirectX::XMFLOAT4) * MAXSIZE, position.data(), sizeof(DirectX::XMFLOAT4) * position.size());
		m_Context->Unmap(m_Buffers[VB_TYPE::POSITION], 0);

		m_Context->Map(m_Buffers[VB_TYPE::COLOR], 0, D3D11_MAP::D3D11_MAP_WRITE_NO_OVERWRITE, 0, &msr);
		ptr4 = (DirectX::XMFLOAT4*)msr.pData;
		memcpy_s(&ptr4[offset[VB_TYPE::COLOR]], sizeof(DirectX::XMFLOAT4) * MAXSIZE, color.data(), sizeof(DirectX::XMFLOAT4) * color.size());
		m_Context->Unmap(m_Buffers[VB_TYPE::COLOR], 0);

		m_Context->Map(m_Buffers[VB_TYPE::TEXCOORD], 0, D3D11_MAP::D3D11_MAP_WRITE_NO_OVERWRITE, 0, &msr);
		ptr2 = (DirectX::XMFLOAT2*)msr.pData;
		memcpy_s(&ptr2[offset[VB_TYPE::TEXCOORD]], sizeof(DirectX::XMFLOAT2) * MAXSIZE, texcoord.data(), sizeof(DirectX::XMFLOAT2) * texcoord.size());
		m_Context->Unmap(m_Buffers[VB_TYPE::TEXCOORD], 0);

		m_Context->Map(m_Buffers[VB_TYPE::NORMAL], 0, D3D11_MAP::D3D11_MAP_WRITE_NO_OVERWRITE, 0, &msr);
		ptr4 = (DirectX::XMFLOAT4*)msr.pData;
		memcpy_s(&ptr4[offset[VB_TYPE::NORMAL]], sizeof(DirectX::XMFLOAT4) * MAXSIZE, normal.data(), sizeof(DirectX::XMFLOAT4) * normal.size());
		m_Context->Unmap(m_Buffers[VB_TYPE::NORMAL], 0);

		m_Context->Map(m_Buffers[VB_TYPE::TANGENT], 0, D3D11_MAP::D3D11_MAP_WRITE_NO_OVERWRITE, 0, &msr);
		ptr4 = (DirectX::XMFLOAT4*)msr.pData;
		memcpy_s(&ptr4[offset[VB_TYPE::TANGENT]], sizeof(DirectX::XMFLOAT4) * MAXSIZE, tangent.data(), sizeof(DirectX::XMFLOAT4) * tangent.size());
		m_Context->Unmap(m_Buffers[VB_TYPE::TANGENT], 0);

		m_Context->Map(m_Buffers[VB_TYPE::BINORMAL], 0, D3D11_MAP::D3D11_MAP_WRITE_NO_OVERWRITE, 0, &msr);
		ptr4 = (DirectX::XMFLOAT4*)msr.pData;
		memcpy_s(&ptr4[offset[VB_TYPE::BINORMAL]], sizeof(DirectX::XMFLOAT4) * MAXSIZE, binormal.data(), sizeof(DirectX::XMFLOAT4) * binormal.size());
		m_Context->Unmap(m_Buffers[VB_TYPE::BINORMAL], 0);

		m_Context->Map(m_Buffers[VB_TYPE::JOINTS], 0, D3D11_MAP::D3D11_MAP_WRITE_NO_OVERWRITE, 0, &msr);
		ptr4 = (DirectX::XMFLOAT4*)msr.pData;
		memcpy_s(&ptr4[offset[VB_TYPE::JOINTS]], sizeof(DirectX::XMFLOAT4) * MAXSIZE, joints.data(), sizeof(DirectX::XMFLOAT4) * joints.size());
		m_Context->Unmap(m_Buffers[VB_TYPE::JOINTS], 0);

		m_Context->Map(m_Buffers[VB_TYPE::WEIGHTS], 0, D3D11_MAP::D3D11_MAP_WRITE_NO_OVERWRITE, 0, &msr);
		ptr4 = (DirectX::XMFLOAT4*)msr.pData;
		memcpy_s(&ptr4[offset[VB_TYPE::WEIGHTS]], sizeof(DirectX::XMFLOAT4) * MAXSIZE, weights.data(), sizeof(DirectX::XMFLOAT4) * weights.size());
		m_Context->Unmap(m_Buffers[VB_TYPE::WEIGHTS], 0);
	}

	for (size_t i = 0; i < VB_TYPE::COUNT; i++)
	{
		m_Offsets[i] += (INT)data.size();
	}
}

ID3D11Buffer * VertexBufferManager::GetVertexBuffer(VB_TYPE type)
{
	return m_Buffers[type];
}

INT* VertexBufferManager::GetOffsets(void)
{
	return m_Offsets;
}
