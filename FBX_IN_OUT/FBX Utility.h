#pragma once
#include "header.h"

namespace FBXINOUT
{
	class FBX
	{
	private:

		Mesh									m_CurrentMesh;
		std::vector<Mesh>						m_Mesh;
		int										m_MeshIndex = 0;

		std::vector<Joint>						m_MeshBindPose;
		std::vector<Animation>					m_Animations;

		bool									m_SystemConverted = false;

		FbxMatrix								m_Rotate180;


		void ResetMesh(Mesh& current_mesh);

		void LoadSkeletonContent(FbxNode * pNode);
		void LoadContent(FbxNode* pNode);
		void LoadMarker(FbxNode* pNode);
		void LoadSkeleton(FbxNode* pNode);
		void LoadMesh(FbxNode* pNode);
		void LoadNurb(FbxNode* pNode);
		void LoadPatch(FbxNode* pNode);
		void LoadCamera(FbxNode* pNode);
		void LoadLight(FbxNode* pNode);
		void LoadLodGroup(FbxNode* pNode);

		void LoadAnimation(FbxAnimStack* pAnimStack, FbxNode* pNode, bool isSwitcher = false);
		void LoadAnimation(FbxAnimLayer* pAnimLayer, FbxNode* pNode, bool isSwitcher = false);
		void LoadAnimation(FbxAudioLayer* pAudioLayer, bool isSwitcher = false);
		

		void DisplayString(const char* pHeader, const char* pValue, const char* pSuffix);
		void LoadPolygons(FbxMesh* pMesh);
		void LoadMaterial(FbxGeometry* pGeometry);
		void LoadTexture(FbxGeometry* pGeometry);
		void LoadLink(FbxGeometry* pGeometry);
		void FindAndDisplayTextureInfoByProperty(FbxProperty pProperty, bool& pDisplayHeader, int pMaterialIndex);
		void LoadMaterialConnections(FbxMesh* pMesh);

		FbxColor FindFbxColor(FbxGeometryElementVertexColor* pElement, int nControlPointIndex, int nVertexId);
		FbxVector2 FindVertexUV(FbxGeometryElementUV* pElement, int nControlPointIndex, int nVertexId);
		FbxVector4 FindVertexNormal(FbxGeometryElementNormal* pElement, int nControlPointIndex, int nVertexId);
		FbxVector4 FindVertexTangent(FbxGeometryElementTangent* pElement, int nControlPointIndex, int nVertexId);
		FbxVector4 FindVertexBinormal(FbxGeometryElementBinormal* pElement, int nControlPointIndex, int nVertexId);
		FbxAMatrix ConvertMatrixToSystemCoord(const FbxAMatrix& matrix);
		FbxDouble DotProductVector4(const FbxVector4& left, const FbxVector4& right);
		FbxVector4 Vector4MultiplyMatrix(const FbxVector4& left, const FbxMatrix& right);
		FbxAMatrix MatrixMultiplyMatrix(const FbxMatrix& left, const FbxMatrix& right);

		std::string ShortenFilePath(std::string filepath);
		void FlattenBindPoseHierarchy(void);
		void RemoveWeakestWeights(Influence& influence);

	public:

		void InitializeSdkObjects(FbxManager*& pManager, FbxScene*& pScene);
		void LoadMetaData(FbxScene* pScene);
		bool LoadScene(FbxManager* pManager, FbxDocument* pScene, const char* pFilename);
		void LoadContent(FbxScene* pScene);
		void LoadBindPose(FbxScene* pScene);
		void LoadAnimation(FbxScene* pScene);
		void LoadAnimationSimple(FbxScene* pScene);
		void DestroySdkObjects(FbxManager* pManager, bool pExitStatus);

		inline std::vector<Mesh> GetMesh(void) { return m_Mesh; }
		inline std::vector<Joint> GetBindPose(void) { return m_MeshBindPose; }
		inline std::vector<Vertex> GetVertices(unsigned int i) { return m_Mesh[i].m_Vertices; }
		inline std::vector<unsigned int> GetIndicies(unsigned int i) { return m_Mesh[i].m_Indicies; }
		inline std::vector<Animation> GetAnimations(void) { return m_Animations; }

		inline void SetSystemConverted(bool sc) { m_SystemConverted = sc; }
	};
};