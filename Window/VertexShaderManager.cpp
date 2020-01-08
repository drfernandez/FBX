#include "VertexShaderManager.h"

BOOL VertexShaderManager::ReadCSOFromFile(const char * pFileName, char ** pByteCode, size_t & nByteCodeSize)
{
	// read the byte code from the .cso
	// and return a buffer with the data and the size
	std::ifstream load;
	load.open(pFileName, std::ios_base::binary);
	if (!load.is_open()) return FALSE;
	load.seekg(0, std::ios_base::end);
	nByteCodeSize = size_t(load.tellg());
	*pByteCode = new char[nByteCodeSize];
	load.seekg(0, std::ios_base::beg);
	load.read(*pByteCode, nByteCodeSize);
	load.close();

	return TRUE;
}

void VertexShaderManager::AddVertexShader(VS_TYPE type, const char * pFilename)
{
	char* byteCode = nullptr;
	size_t byteCodeSize = 0;
	BOOL success = ReadCSOFromFile(pFilename, &byteCode, byteCodeSize);

	if (success)
	{
		m_Device->CreateVertexShader(byteCode, byteCodeSize, nullptr, &m_Shaders[type]);

		delete[] byteCode;
	}

}

VertexShaderManager * VertexShaderManager::GetInstance(void)
{
	static VertexShaderManager instance;
	return &instance;
}

void VertexShaderManager::Initialize(ID3D11Device * pDevice)
{
	// pointers to device
	// DO NOT RELEASE in this class
	m_Device = pDevice;

	AddVertexShader(VS_TYPE::PCTNTBJW,		"../Shaders/VS_PCTNTBJW.cso");
	AddVertexShader(VS_TYPE::PCTNTBJW_ANIM, "../Shaders/VS_PCTNTBJW_ANIM.cso");
	AddVertexShader(VS_TYPE::DEBUGLINES,	"../Shaders/VS_DEBUGLINES.cso");
	AddVertexShader(VS_TYPE::SKYBOX,		"../Shaders/VS_SKYBOX.cso");
}

void VertexShaderManager::Shutdown(void)
{
	for (size_t i = 0; i < VS_TYPE::COUNT; i++)
	{
		SAFE_RELEASE(m_Shaders[i]);
	}
}

ID3D11VertexShader * VertexShaderManager::GetVertexShader(VS_TYPE type)
{
	return m_Shaders[type];
}
