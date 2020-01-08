#pragma once
#include <fbxsdk.h>
#include <functional>

struct Vertex
{
	FbxVector4				position;
	FbxColor				color;
	FbxVector2				texcoord;
	FbxVector4				normal;
	FbxVector4				tangent;
	FbxVector4				binormal;
	int						control_point_index;
	FbxVector4				joint;
	FbxVector4				weight;
};

struct VertexHash
{
	std::size_t operator()(const Vertex& v) const
	{
		return size_t(
			std::hash<FbxDouble>()(v.position[0]) +
			std::hash<FbxDouble>()(v.position[1]) +
			std::hash<FbxDouble>()(v.position[2]) +
			std::hash<FbxDouble>()(v.position[3]) +
			std::hash<FbxDouble>()(v.color[0]) +
			std::hash<FbxDouble>()(v.color[1]) +
			std::hash<FbxDouble>()(v.color[2]) +
			std::hash<FbxDouble>()(v.color[3]) +
			std::hash<FbxDouble>()(v.texcoord[0]) +
			std::hash<FbxDouble>()(v.texcoord[1]) +
			std::hash<FbxDouble>()(v.normal[0]) +
			std::hash<FbxDouble>()(v.normal[1]) +
			std::hash<FbxDouble>()(v.normal[2]) +
			std::hash<FbxDouble>()(v.normal[3]) +
			std::hash<FbxDouble>()(v.tangent[0]) +
			std::hash<FbxDouble>()(v.tangent[1]) +
			std::hash<FbxDouble>()(v.tangent[2]) +
			std::hash<FbxDouble>()(v.tangent[3]) +
			std::hash<FbxDouble>()(v.binormal[0]) +
			std::hash<FbxDouble>()(v.binormal[1]) +
			std::hash<FbxDouble>()(v.binormal[2]) +
			std::hash<FbxDouble>()(v.binormal[3]) +
			std::hash<FbxDouble>()(v.joint[0]) +
			std::hash<FbxDouble>()(v.joint[1]) +
			std::hash<FbxDouble>()(v.joint[2]) +
			std::hash<FbxDouble>()(v.joint[3]) +
			std::hash<FbxDouble>()(v.weight[0]) +
			std::hash<FbxDouble>()(v.weight[1]) +
			std::hash<FbxDouble>()(v.weight[2]) +
			std::hash<FbxDouble>()(v.weight[3])
		);
	}
};

struct VertexEqual
{
	bool operator()(const Vertex& v1, const Vertex& v2) const
	{
		return (v1.position == v2.position) &&
			(v1.color == v2.color) &&
			(v1.texcoord == v2.texcoord) &&
			(v1.normal == v2.normal) &&
			(v1.tangent == v2.tangent) &&
			(v1.binormal == v2.binormal) &&
			(v1.weight == v2.weight) &&
			(v1.joint == v2.joint);
	}
};

struct Joint
{
	FbxNode*			node;
	FbxString			name;
	FbxMatrix			matrix;
	FbxNode*			parent_node;
	int					parent_index;
};

struct KeyFrame
{
	FbxFloat					time;
	std::vector<Joint>			joints;
};

struct Animation
{
	FbxString					name;
	FbxFloat					duration;
	std::vector<KeyFrame>		keyframes;
};

struct Influence
{
	std::vector<unsigned int>		joint;
	std::vector<float>				weight;
};

struct Mesh
{
	std::string					m_MeshName = "";
	std::vector<Vertex>			m_Vertices;
	std::vector<unsigned int>	m_Indicies;
	int							m_VertOffset = -1;
	int							m_VertTotal = -1;
	int							m_IndexOffset = -1;
	int							m_IndexTotal = -1;
	FbxMatrix					m_WorldMatrix;
	FbxString					m_TextureDiffuseName = "";
	int							m_TextureDiffuseID = -1;
	FbxString					m_TextureNormalName = "";
	int							m_TextureNormalID = -1;
	FbxString					m_TextureSpecularName = "";
	int							m_TextureSpecularID = -1;
	std::vector<Influence>		m_Influences;

	Mesh() = default;
	~Mesh() = default;
	Mesh(const Mesh& copy) = default;
	Mesh& operator=(const Mesh& copy) = default;
};