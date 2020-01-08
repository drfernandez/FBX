#include "DebugRenderer.h"

DebugRenderer * DebugRenderer::GetInstance(void)
{
	static DebugRenderer instance;
	return &instance;
}

void DebugRenderer::Initialize(void)
{
}

void DebugRenderer::Shutdown(void)
{
}

DirectX::XMVECTOR * DebugRenderer::GetData(void)
{
	return m_Array.data();
}

size_t DebugRenderer::GetCapacity(void)
{
	return size_t(MAXCOUNT);
}

size_t DebugRenderer::GetVertexCount(void)
{
	return size_t(m_VertCount);
}

void DebugRenderer::AddLines(const DirectX::XMVECTOR & start, const DirectX::XMVECTOR & end)
{
	m_Array[m_VertCount++] = start;
	m_Array[m_VertCount++] = end;
}

void DebugRenderer::ClearLines(void)
{
	m_VertCount = 0;
}
