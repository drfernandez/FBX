#include "InputLayoutManager.h"

BOOL InputLayoutManager::ReadCSOFromFile(const char * pFileName, char ** pByteCode, size_t & nByteCodeSize)
{
	// read the byte code from the .cso
	// and return a buffer with the data and the size
	std::ifstream load;
	load.open(pFileName, std::ios_base::binary);
	if (!load.is_open()) return false;
	load.seekg(0, std::ios_base::end);
	nByteCodeSize = size_t(load.tellg());
	*pByteCode = new char[nByteCodeSize];
	load.seekg(0, std::ios_base::beg);
	load.read(*pByteCode, nByteCodeSize);
	load.close();

	return true;
}

void InputLayoutManager::AddInputLayout(IL_TYPE type, const char * pFilename)
{
	HRESULT hr = 0;
	char* byteCode = nullptr;
	size_t byteCodeSize = 0;
	BOOL success = ReadCSOFromFile(pFilename, &byteCode, byteCodeSize);

	if (success)
	{
		switch (type)
		{
		case InputLayoutManager::PCTNTBJW:
			{
				D3D11_INPUT_ELEMENT_DESC element[] =
				{
					{ "POSITION", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0 },
					{ "COLOR",	  0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0 },
					{ "TEXCOORD", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT,       2, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0 },
					{ "NORMAL",   0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 3, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0 },
					{ "TANGENT",  0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 4, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0 },
					{ "BINORMAL", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 5, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0 },
					{ "JOINTS",	  0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 6, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0 },
					{ "WEIGHTS",  0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 7, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0 }
				};

				hr = m_Device->CreateInputLayout(element, ARRAYSIZE(element), byteCode, byteCodeSize, &m_Layouts[type]);
			}
			break;
		case InputLayoutManager::COUNT:
			break;
		default:
			break;
		}

		delete[] byteCode;
	}
}

InputLayoutManager * InputLayoutManager::GetInstance(void)
{
	static InputLayoutManager instance;
	return &instance;
}

void InputLayoutManager::Initialize(ID3D11Device * pDevice)
{
	// pointers to device and context
	// DO NOT RELEASE in this class
	m_Device = pDevice;

	AddInputLayout(IL_TYPE::PCTNTBJW, "../Shaders/VS_PCTNTBJW.cso");
}

void InputLayoutManager::Shutdown(void)
{
	for (size_t i = 0; i < IL_TYPE::COUNT; i++)
	{
		SAFE_RELEASE(m_Layouts[i]);
	}
}

ID3D11InputLayout * InputLayoutManager::GetInputLayout(IL_TYPE type)
{
	return m_Layouts[type];
}
