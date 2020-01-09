#include "SceneManager.h"

void SceneManager::LoadStaticModel(const std::string & modelname, const std::string & meshname)
{

}

void SceneManager::LoadDynamicModel(const std::string & name, const std::string & mesh, const std::string & pose, const std::vector<std::string>& animation)
{

}

SceneManager * SceneManager::GetInstance(void)
{
	static SceneManager instance;
	return &instance;
}

void SceneManager::Initialize(void)
{

}

void SceneManager::LoadScene(const std::string & filename)
{

}

void SceneManager::Shutdown(void)
{

}
