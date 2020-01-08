#pragma once
#include "header.h"
#include <array>

#define MAXCOUNT 8096

class DebugRenderer
{
public:

private:
	// proper singleton closure
	DebugRenderer() = default;
	~DebugRenderer() = default;
	DebugRenderer(const DebugRenderer& copy) = default;
	DebugRenderer& operator=(const DebugRenderer& copy) = default;

	// data members
	size_t							m_VertCount = 0;

	std::array<DirectX::XMVECTOR, MAXCOUNT>	m_Array;

public:
	static DebugRenderer* GetInstance(void);

	void Initialize(void);
	void Shutdown(void);

	DirectX::XMVECTOR* GetData(void);
	size_t GetCapacity(void);
	size_t GetVertexCount(void);
	void AddLines(const DirectX::XMVECTOR& start, const DirectX::XMVECTOR& end);
	void ClearLines(void);
};