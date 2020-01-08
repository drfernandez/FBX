#include "TextureManager.h"
#include "DDSTextureLoader.h"
#include "WICTextureLoader.h"


std::wstring TextureManager::to_wstring(std::string const & str)
{
	//setup converter
	typedef std::codecvt_utf8<wchar_t> convert_type;
	std::wstring_convert<convert_type, wchar_t> converter;
	//use converter (.to_bytes: wstr->str, .from_bytes: str->wstr)
	return converter.from_bytes(str);
}

int TextureManager::ParseFileExtension(const std::string & filename)
{
	std::string delimeter = ".";
	std::string path = filename;

	size_t pos = 0;
	std::string token;
	while ((pos = path.find(delimeter)) != std::string::npos)
	{
		token = path.substr(0, pos);
		path.erase(0, pos + delimeter.length());
	}

	if(path == "dds")
	{
		return 0;
	}

	return 1;
}

TextureManager * TextureManager::GetInstance(void)
{
	static TextureManager instance;
	return &instance;
}

void TextureManager::Initialize(ID3D11Device * pDevice, ID3D11DeviceContext* pContext)
{
	m_Device = pDevice;
	m_Context = pContext;
}

void TextureManager::Shutdown(void)
{
	for (size_t i = 0; i < m_Diffuse.size(); i++)
	{
		SAFE_RELEASE(m_Diffuse[i]);
	}
	for (size_t i = 0; i < m_Normal.size(); i++)
	{
		SAFE_RELEASE(m_Normal[i]);
	}
	for (size_t i = 0; i < m_Specular.size(); i++)
	{
		SAFE_RELEASE(m_Specular[i]);
	}
}

INT TextureManager::AddTexture(const std::string & pFilename, TM_TYPE type)
{
	ID3D11ShaderResourceView* srv = nullptr;
	HRESULT hr = 0;
	INT index = -1;

	switch (type)
	{
	case TextureManager::DIFFUSE:
		m_DiffuseSearchMapIter = m_DiffuseSearchMap.find(pFilename);
		if (m_DiffuseSearchMapIter == m_DiffuseSearchMap.end())
		{
			index = (INT)m_Diffuse.size();
			m_DiffuseSearchMap[pFilename] = index;

			switch (ParseFileExtension(pFilename))
			{
			case 0:
				hr = DirectX::CreateDDSTextureFromFile(m_Device, m_Context, to_wstring(pFilename).c_str(), nullptr, &srv);
				break;
			case 1:
				hr = DirectX::CreateWICTextureFromFile(m_Device, m_Context, to_wstring(pFilename).c_str(), nullptr, &srv);
				break;
			default:
				break;
			};

			m_Diffuse.push_back(srv);
		}
		else
		{
			index = m_DiffuseSearchMapIter->second;
		}
		break;
	case TextureManager::NORMAL:
		m_NormalSearchMapIter = m_NormalSearchMap.find(pFilename);
		if (m_NormalSearchMapIter == m_NormalSearchMap.end())
		{
			index = (INT)m_Normal.size();
			m_NormalSearchMap[pFilename] = index;

			switch (ParseFileExtension(pFilename))
			{
			case 0:
				hr = DirectX::CreateDDSTextureFromFile(m_Device, m_Context, to_wstring(pFilename).c_str(), nullptr, &srv);
				break;
			case 1:
				hr = DirectX::CreateWICTextureFromFile(m_Device, m_Context, to_wstring(pFilename).c_str(), nullptr, &srv);
				break;
			default:
				break;
			};

			m_Normal.push_back(srv);
		}
		else
		{
			index = m_NormalSearchMapIter->second;
		}
		break;
	case TextureManager::SPECULAR:
		m_SpecularSearchMapIter = m_SpecularSearchMap.find(pFilename);
		if (m_SpecularSearchMapIter == m_SpecularSearchMap.end())
		{
			index = (INT)m_Specular.size();
			m_SpecularSearchMap[pFilename] = index;

			switch (ParseFileExtension(pFilename))
			{
			case 0:
				hr = DirectX::CreateDDSTextureFromFile(m_Device, m_Context, to_wstring(pFilename).c_str(), nullptr, &srv);
				break;
			case 1:
				hr = DirectX::CreateWICTextureFromFile(m_Device, m_Context, to_wstring(pFilename).c_str(), nullptr, &srv);
				break;
			default:
				break;
			};

			m_Specular.push_back(srv);
		}
		else
		{
			index = m_SpecularSearchMapIter->second;
		}
		break;
	case TextureManager::COUNT:
		break;
	default:
		break;
	}
	return index;
}

ID3D11ShaderResourceView * TextureManager::GetShaderResourceView(TM_TYPE type, INT id)
{
	ID3D11ShaderResourceView* return_value = nullptr;

	switch (type)
	{
	case TextureManager::DIFFUSE:
		if (id >= 0 && id < m_Diffuse.size())
		{
			return_value = m_Diffuse[id];
		}
		break;
	case TextureManager::NORMAL:
		if (id >= 0 && id < m_Normal.size())
		{
			return_value = m_Normal[id];
		}
		break;
	case TextureManager::SPECULAR:
		if (id >= 0 && id < m_Specular.size())
		{
			return_value = m_Specular[id];
		}
		break;
	case TextureManager::COUNT:
		break;
	default:
		break;
	}
	return return_value;
}
