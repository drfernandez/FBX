#include "PixelShaderManager.h"

BOOL PixelShaderManager::ReadCSOFromFile(const char * pFileName, char ** pByteCode, size_t & nByteCodeSize)
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

void PixelShaderManager::AddPixelShader(PS_TYPE type, const char * pFilename)
{
	char* byteCode = nullptr;
	size_t byteCodeSize = 0;
	BOOL success = ReadCSOFromFile(pFilename, &byteCode, byteCodeSize);

	if (success)
	{
		m_Device->CreatePixelShader(byteCode, byteCodeSize, nullptr, &m_Shaders[type]);

		delete[] byteCode;
	}
}

PixelShaderManager * PixelShaderManager::GetInstance(void)
{
	static PixelShaderManager instance;
	return &instance;
}

void PixelShaderManager::Initialize(ID3D11Device * pDevice)
{
	// pointers to device
	// DO NOT RELEASE in this class
	m_Device = pDevice;

	AddPixelShader(PS_TYPE::PCTNTBJW,			"../Shaders/PS_PCTNTBJW.cso");
	AddPixelShader(PS_TYPE::PCTNTBJW_TEXONLY,	"../Shaders/PS_PCTNTBJW_TEX_ONLY.cso");
	AddPixelShader(PS_TYPE::DEBUGLINES,			"../Shaders/PS_DEBUGLINES.cso");
	AddPixelShader(PS_TYPE::SKYBOX,				"../Shaders/PS_SKYBOX.cso");
}

void PixelShaderManager::Shutdown(void)
{
	for (size_t i = 0; i < PS_TYPE::COUNT; i++)
	{
		SAFE_RELEASE(m_Shaders[i]);
	}
}

ID3D11PixelShader * PixelShaderManager::GetPixelShader(PS_TYPE type)
{
	return m_Shaders[type];
}
