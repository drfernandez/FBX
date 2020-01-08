#pragma once
#include "header.h"


class BinaryReader
{
private:

	std::fstream read;

	BinaryReader() = default;
	~BinaryReader() = default;
	BinaryReader(const BinaryReader& copy) = default;
	BinaryReader& operator=(const BinaryReader& copy) = default;

public:
	static BinaryReader* GetInstance(void);
	void Initialize(void);
	void Shutdown(void);

	bool ReadMeshFromFile(const char* filename, std::vector<Mesh*>& m);
	bool ReadBindPoseFromFile(const char* filename, std::vector<Joint*>& j);
	bool ReadAnimationsFromFile(const char* filename, std::vector<Animation*>& a);
};

