#pragma once

#include <vector>
#include <DirectXMath.h>
#include "Vertex.h"
#include "Mesh.h"

#define SAFE_DELETE(ptr) { if(ptr) { delete ptr; ptr = nullptr; } }

struct VIEW_PROJ
{
	DirectX::XMMATRIX view;
	DirectX::XMMATRIX proj;
};

class Joint
{
public:
	std::string					name;
	DirectX::XMMATRIX			matrix;
	int							parent_index;

private:

public:
	Joint(void);
	~Joint(void);
	Joint(const Joint& copy) = delete;
	Joint& operator=(const Joint& copy) = delete;
};

class KeyFrame
{
public:

	float						time;
	std::vector<Joint*>			joints;

private:

public:
	KeyFrame(void);
	~KeyFrame(void);
	KeyFrame(const KeyFrame& copy) = delete;
	KeyFrame& operator=(const KeyFrame& copy) = delete;

};

class Animation
{
public:
	std::string					name;
	float						duration;
	std::vector<KeyFrame*>		keyframes;

private:

public:
	Animation(void);
	~Animation(void);
	Animation(const Animation& copy) = delete;
	Animation& operator=(const Animation& copy) = delete;
};

struct LIGHT
{
	DirectX::XMFLOAT4	position;
	DirectX::XMFLOAT4	color;
	DirectX::XMFLOAT4	direction;
	DirectX::XMFLOAT4	cone_ratio;
};

struct CB_LIGHT
{
	DirectX::XMFLOAT4	data;
	LIGHT				array[100];
};