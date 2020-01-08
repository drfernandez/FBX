#include "DynamicModel.h"


bool DynamicModel::LoadBindPose(const std::string & pFilename, std::vector<Joint*>& bindpose)
{
	BOOL success = false;
	success = m_BinaryReader->ReadBindPoseFromFile(pFilename.c_str(), bindpose);
	return success;
}

bool DynamicModel::LoadAnimations(const std::string & pFilename, std::vector<Animation*>& animations)
{
	BOOL success = false;
	success = m_BinaryReader->ReadAnimationsFromFile(pFilename.c_str(), animations);
	return success;
}

DynamicModel::DynamicModel(void) : StaticModel()
{
	m_BindPose.clear();
	m_Animations.clear();
	m_Interpolator.SetAnimation(nullptr);
	m_CurrentPose.clear(); 
}

DynamicModel::DynamicModel(BinaryReader * reader, VertexBufferManager * vbm, IndexBufferManager * ibm, TextureManager * tm) :
	StaticModel(reader, vbm, ibm, tm)
{
	m_BindPose.clear();
	m_Animations.clear();
	m_Interpolator.SetAnimation(nullptr);
	m_CurrentPose.clear();
}

DynamicModel::~DynamicModel(void)
{
	for (size_t i = 0; i < m_BindPose.size(); i++)
	{
		SAFE_DELETE(m_BindPose[i]);
	}
	m_BindPose.clear();
	for (size_t i = 0; i < m_Animations.size(); i++)
	{
		SAFE_DELETE(m_Animations[i]);
	}
	m_Animations.clear();
	m_Interpolator.SetAnimation(nullptr);
	for (size_t i = 0; i < m_CurrentPose.size(); i++)
	{
		SAFE_DELETE(m_CurrentPose[i]);
	}
	m_CurrentPose.clear();
}

//DynamicModel::DynamicModel(const DynamicModel & copy) : StaticModel(copy)
//{
//	for (size_t i = 0; i < copy.m_BindPose.size(); i++)
//	{
//		Joint* temp_joint = new Joint();
//		memcpy_s(temp_joint, sizeof(copy.m_BindPose[i]), copy.m_BindPose[i], sizeof(copy.m_BindPose[i]));
//		m_BindPose.push_back(temp_joint);
//	}
//	for (size_t i = 0; i < copy.m_Animations.size(); i++)
//	{
//		Animation* temp_anim = new Animation();
//		temp_anim->duration = copy.m_Animations[i]->duration;
//		temp_anim->name = copy.m_Animations[i]->name;
//		for (size_t j = 0; j < copy.m_Animations[i]->keyframes.size(); j++)
//		{
//			KeyFrame* temp_kf = new KeyFrame();
//			temp_kf->time = copy.m_Animations[i]->keyframes[j]->time;
//			for (size_t m = 0; m < copy.m_Animations[i]->keyframes[j]->joints.size(); m++)
//			{
//				Joint* temp_joint = new Joint();
//				memcpy_s(temp_joint, sizeof(copy.m_Animations[i]->keyframes[j]->joints[m]), copy.m_Animations[i]->keyframes[j]->joints[m], sizeof(copy.m_Animations[i]->keyframes[j]->joints[m]));
//				temp_kf->joints.push_back(temp_joint);
//			}
//			temp_anim->keyframes.push_back(temp_kf);
//		}
//		m_Animations.push_back(temp_anim);
//	}
//	m_Interpolator = copy.m_Interpolator;
//	m_Interpolator.SetAnimation(m_Animations[0]);
//
//	for (size_t i = 0; i < copy.m_CurrentPose.size(); i++)
//	{
//		Joint* temp_joint = new Joint();
//		memcpy_s(temp_joint, sizeof(copy.m_CurrentPose[i]), copy.m_CurrentPose[i], sizeof(copy.m_CurrentPose[i]));
//		m_CurrentPose.push_back(temp_joint);
//	}
//}
//
//DynamicModel & DynamicModel::operator=(const DynamicModel & copy)
//{
//	if (this != &copy)
//	{
//		for (size_t i = 0; i < copy.m_BindPose.size(); i++)
//		{
//			Joint* temp_joint = new Joint();
//			memcpy_s(temp_joint, sizeof(copy.m_BindPose[i]), copy.m_BindPose[i], sizeof(copy.m_BindPose[i]));
//			m_BindPose.push_back(temp_joint);
//		}
//		for (size_t i = 0; i < copy.m_Animations.size(); i++)
//		{
//			Animation* temp_anim = new Animation();
//			temp_anim->duration = copy.m_Animations[i]->duration;
//			temp_anim->name = copy.m_Animations[i]->name;
//			for (size_t j = 0; j < copy.m_Animations[i]->keyframes.size(); j++)
//			{
//				KeyFrame* temp_kf = new KeyFrame();
//				temp_kf->time = copy.m_Animations[i]->keyframes[j]->time;
//				for (size_t m = 0; m < copy.m_Animations[i]->keyframes[j]->joints.size(); m++)
//				{
//					Joint* temp_joint = new Joint();
//					memcpy_s(temp_joint, sizeof(copy.m_Animations[i]->keyframes[j]->joints[m]), copy.m_Animations[i]->keyframes[j]->joints[m], sizeof(copy.m_Animations[i]->keyframes[j]->joints[m]));
//					temp_kf->joints.push_back(temp_joint);
//				}
//				temp_anim->keyframes.push_back(temp_kf);
//			}
//			m_Animations.push_back(temp_anim);
//		}
//		m_Interpolator = copy.m_Interpolator;
//		if (m_Animations.size() > 0)
//		{
//			m_Interpolator.SetAnimation(m_Animations[0]);
//		}
//		else
//		{
//			m_Interpolator.SetAnimation(nullptr);
//		}
//
//		for (size_t i = 0; i < copy.m_CurrentPose.size(); i++)
//		{
//			Joint* temp_joint = new Joint();
//			memcpy_s(temp_joint, sizeof(copy.m_CurrentPose[i]), copy.m_CurrentPose[i], sizeof(copy.m_CurrentPose[i]));
//			m_CurrentPose.push_back(temp_joint);
//		}
//	}
//	return *this;
//}

void DynamicModel::LoadModelFromFile(std::string szMeshFilename,
	std::string szBindposeFilename,
	std::vector<std::string> szAnimationFilenames)
{
	LoadMesh(szMeshFilename, m_Meshes);
	LoadBindPose(szBindposeFilename, m_BindPose);
	for (size_t i = 0; i < szAnimationFilenames.size(); i++)
	{
		LoadAnimations(szAnimationFilenames[i], m_Animations);
	}
	if (m_Animations.size() > 0)
	{
		m_Interpolator.SetAnimation(m_Animations[0]);
		m_Interpolator.SetTimeCurrent(0.0f);
	}
	else
	{
		m_Interpolator.SetAnimation(nullptr);
		m_Interpolator.SetTimeCurrent(0.0f);
	}
	for (size_t i = 0; i < m_BindPose.size(); i++)
	{
		Joint* temp_joint = new Joint();
		m_CurrentPose.push_back(temp_joint);
	}
}
