#pragma once

#include <DirectXMath.h>


struct Vertex
{
	DirectX::XMFLOAT4	position;
	DirectX::XMFLOAT4	color;
	DirectX::XMFLOAT2	texcoord;
	DirectX::XMFLOAT4	normal;
	DirectX::XMFLOAT4	tangent;
	DirectX::XMFLOAT4	binormal;
	DirectX::XMFLOAT4	joint;
	DirectX::XMFLOAT4	weight;
};

struct VertexHash
{
	std::size_t operator()(const Vertex& v) const
	{
		return size_t(
			std::hash<float>()(v.position.x) +
			std::hash<float>()(v.position.y) +
			std::hash<float>()(v.position.z) +
			std::hash<float>()(v.position.w) +
			std::hash<float>()(v.color.x) +
			std::hash<float>()(v.color.y) +
			std::hash<float>()(v.color.z) +
			std::hash<float>()(v.color.w) +
			std::hash<float>()(v.texcoord.x) +
			std::hash<float>()(v.texcoord.y) +
			std::hash<float>()(v.normal.x) +
			std::hash<float>()(v.normal.y) +
			std::hash<float>()(v.normal.z) +
			std::hash<float>()(v.normal.w) +
			std::hash<float>()(v.tangent.x) +
			std::hash<float>()(v.tangent.y) +
			std::hash<float>()(v.tangent.z) +
			std::hash<float>()(v.tangent.w) +
			std::hash<float>()(v.binormal.x) +
			std::hash<float>()(v.binormal.y) +
			std::hash<float>()(v.binormal.z) +
			std::hash<float>()(v.binormal.w) +
			std::hash<float>()(v.joint.x) +
			std::hash<float>()(v.joint.y) +
			std::hash<float>()(v.joint.z) +
			std::hash<float>()(v.joint.w) +
			std::hash<float>()(v.weight.x) +
			std::hash<float>()(v.weight.y) +
			std::hash<float>()(v.weight.z) +
			std::hash<float>()(v.weight.w)
		);
	}
};

struct VertexEqual
{
	bool operator()(const Vertex& v1, const Vertex& v2) const
	{
		return bool(
			(v1.position.x == v2.position.x) &&
			(v1.position.y == v2.position.y) &&
			(v1.position.z == v2.position.z) &&
			(v1.position.w == v2.position.w) &&
			(v1.color.x == v2.color.x) &&
			(v1.color.y == v2.color.y) &&
			(v1.color.z == v2.color.z) &&
			(v1.color.w == v2.color.w) &&
			(v1.texcoord.x == v2.texcoord.x) &&
			(v1.texcoord.y == v2.texcoord.y) &&
			(v1.normal.x == v2.normal.x) &&
			(v1.normal.y == v2.normal.y) &&
			(v1.normal.z == v2.normal.z) &&
			(v1.normal.w == v2.normal.w) &&
			(v1.tangent.x == v2.tangent.x) &&
			(v1.tangent.y == v2.tangent.y) &&
			(v1.tangent.z == v2.tangent.z) &&
			(v1.tangent.w == v2.tangent.w) &&
			(v1.binormal.x == v2.binormal.x) &&
			(v1.binormal.y == v2.binormal.y) &&
			(v1.binormal.z == v2.binormal.z) &&
			(v1.binormal.w == v2.binormal.w) &&
			(v1.joint.x == v2.joint.x) &&
			(v1.joint.y == v2.joint.y) &&
			(v1.joint.z == v2.joint.z) &&
			(v1.joint.w == v2.joint.w) &&
			(v1.weight.x == v2.weight.x) &&
			(v1.weight.y == v2.weight.y) &&
			(v1.weight.z == v2.weight.z) &&
			(v1.weight.w == v2.weight.w)
			);
	}
};