#include "Structures.h"


Joint::Joint(void)
{
	name = "";
	matrix = DirectX::XMMatrixIdentity();
	parent_index = -1;
}

Joint::~Joint(void)
{
	name = "";
	matrix = DirectX::XMMatrixIdentity();
	parent_index = -1;
}

//Joint::Joint(const Joint & copy)
//{
//	name = copy.name;
//	matrix = copy.matrix;
//	parent_index = copy.parent_index;
//}
//
//Joint & Joint::operator=(const Joint & copy)
//{
//	if (this != &copy)
//	{
//		name = copy.name;
//		matrix = copy.matrix;
//		parent_index = copy.parent_index;
//	}
//	return *this;
//}

KeyFrame::KeyFrame(void)
{
	this->time = 0.0f;
	this->joints.clear();
}

KeyFrame::~KeyFrame(void)
{
	this->time = 0.0f;
	for (size_t i = 0; i < joints.size(); i++)
	{
		SAFE_DELETE(joints[i]);
	}
}

//KeyFrame::KeyFrame(const KeyFrame & copy)
//{
//	time = copy.time;
//	for (size_t i = 0; i < copy.joints.size(); i++)
//	{
//		Joint* temp_joint = new Joint();
//		memcpy_s(temp_joint, sizeof(copy.joints[i]), copy.joints[i], sizeof(copy.joints[i]));
//		joints.push_back(temp_joint);
//	}
//}
//
//KeyFrame & KeyFrame::operator=(const KeyFrame & copy)
//{
//	if (this != &copy)
//	{
//		time = copy.time;
//		for (size_t i = 0; i < copy.joints.size(); i++)
//		{
//			Joint* temp_joint = new Joint();
//			memcpy_s(temp_joint, sizeof(copy.joints[i]), copy.joints[i], sizeof(copy.joints[i]));
//			joints.push_back(temp_joint);
//		}
//	}
//	return *this;
//}

Animation::Animation(void)
{
	name = "";
	duration = 0.0f;
	keyframes.clear();
}

Animation::~Animation(void)
{
	name = "";
	duration = 0.0f;
	for (size_t i = 0; i < keyframes.size(); i++)
	{
		SAFE_DELETE(keyframes[i]);
	}
}

//Animation::Animation(const Animation & copy)
//{
//	name = copy.name;
//	duration = copy.duration;
//	for (size_t j = 0; j < copy.keyframes.size(); j++)
//	{
//		KeyFrame* kf = new KeyFrame();
//		kf->time = copy.keyframes[j]->time;
//		for (size_t i = 0; i < copy.keyframes[j]->joints.size(); i++)
//		{
//			Joint* temp_joint = new Joint();
//			memcpy_s(temp_joint, sizeof(copy.keyframes[j]->joints[i]), copy.keyframes[j]->joints[i], sizeof(copy.keyframes[j]->joints[i]));
//			kf->joints.push_back(temp_joint);
//		}
//		keyframes.push_back(kf);
//	}
//}
//
//Animation & Animation::operator=(const Animation & copy)
//{
//	if (this != &copy)
//	{
//		name = copy.name;
//		duration = copy.duration;
//		for (size_t j = 0; j < copy.keyframes.size(); j++)
//		{
//			KeyFrame* kf = new KeyFrame();
//			kf->time = copy.keyframes[j]->time;
//			for (size_t i = 0; i < copy.keyframes[j]->joints.size(); i++)
//			{
//				Joint* temp_joint = new Joint();
//				memcpy_s(temp_joint, sizeof(copy.keyframes[j]->joints[i]), copy.keyframes[j]->joints[i], sizeof(copy.keyframes[j]->joints[i]));
//				kf->joints.push_back(temp_joint);
//			}
//			keyframes.push_back(kf);
//		}
//	}
//	return *this;
//}
