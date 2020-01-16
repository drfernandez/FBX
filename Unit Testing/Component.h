#pragma once
#include <array>

class Component
{
private:
	
public:
	Component() { }
	virtual ~Component() { }

};

using float4 = std::array<float, 4>;
using float4x4 = std::array<float4, 4>;

class TransformComponent : public Component
{
private:
	float4x4 matrix;

public:
	TransformComponent() : matrix({ 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1 } ) { }
	virtual ~TransformComponent() override { }

	inline float4x4 get_matrix() { return matrix; }
	inline void set_matrix(float4x4 m) { matrix = m; }

};