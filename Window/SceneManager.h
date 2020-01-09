#pragma once

#include "header.h"
#include "ModelManager.h"


class SceneManager
{
public:

private:
	// proper singleton closure
	SceneManager() = default;
	~SceneManager() = default;
	SceneManager(const SceneManager& copy) = default;
	SceneManager& operator=(const SceneManager& copy) = default;

	void LoadStaticModel(const std::string& name, const std::string& mesh);
	void LoadDynamicModel(const std::string& name, const std::string& mesh, const std::string& pose, const std::vector<std::string>& animation);

public:
	// singleton access
	static SceneManager* GetInstance(void);

	// public functions
	void Initialize(void);
	void LoadScene(const std::string& filename);
	void Shutdown(void);
};