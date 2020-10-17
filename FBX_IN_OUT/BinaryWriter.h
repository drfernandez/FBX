#pragma once

#include <vector>
#include <fstream>
#include "Structures.h"

class BinaryWriter
{
private:

	std::fstream write;

	BinaryWriter() = default;
	~BinaryWriter() = default;
	BinaryWriter(const BinaryWriter& copy) = default;
	BinaryWriter& operator=(const BinaryWriter& copy) = default;

	void WriteMatrix(const FbxMatrix& m);
	void WriteQuaternion(const FbxQuaternion& q);

public:
	static BinaryWriter* GetInstance(void);
	void Initialize(void);
	void Shutdown(void);

	bool WriteMeshToFile(const char* filename, const std::vector<Mesh>& m);
	bool WriteBindPoseToFile(const char* filename, const std::vector<Joint>& j);
	bool WriteAnimationsToFile(const char* filename, const std::vector<Animation>& a);
};