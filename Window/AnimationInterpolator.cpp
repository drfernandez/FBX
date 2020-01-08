#include "AnimationInterpolator.h"

void AnimationInterpolator::AddTime(float t)
{
	m_fCurrentTime += t;
	if (m_fCurrentTime >= m_fDuration)
	{
		m_fCurrentTime = 0.0f;
	}
}

void AnimationInterpolator::SetTime(float t)
{
	m_fCurrentTime = t;
}

void AnimationInterpolator::SetDuration(float t)
{
	m_fDuration = t;
}

float AnimationInterpolator::LinearInterpolation(const float & a, const float & b, float ratio)
{
	return (b - a) * ratio + a;
}

DirectX::XMVECTOR AnimationInterpolator::InterpolatePosition(const DirectX::XMVECTOR & a, const DirectX::XMVECTOR & b, float ratio)
{
	DirectX::XMVECTOR return_vector = DirectX::XMVectorSet(
		LinearInterpolation(a.m128_f32[0], b.m128_f32[0], ratio),
		LinearInterpolation(a.m128_f32[1], b.m128_f32[1], ratio),
		LinearInterpolation(a.m128_f32[2], b.m128_f32[2], ratio),
		1.0f);

	return return_vector;
}

DirectX::XMMATRIX AnimationInterpolator::InterpolateMatrix(const DirectX::XMMATRIX & a, const DirectX::XMMATRIX & b, float ratio)
{
	DirectX::XMMATRIX temp_a = DirectX::XMMATRIX(a.r[0], a.r[1], a.r[2], DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f));
	DirectX::XMMATRIX temp_b = DirectX::XMMATRIX(b.r[0], b.r[1], b.r[2], DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f));

	DirectX::XMVECTOR quaternion_a = DirectX::XMQuaternionNormalize(DirectX::XMQuaternionRotationMatrix(temp_a));
	DirectX::XMVECTOR quaternion_b = DirectX::XMQuaternionNormalize(DirectX::XMQuaternionRotationMatrix(temp_b));
	DirectX::XMVECTOR quaternion_c = DirectX::XMQuaternionNormalize(DirectX::XMQuaternionSlerp(quaternion_a, quaternion_b, ratio));

	DirectX::XMMATRIX temp_c = DirectX::XMMatrixRotationQuaternion(quaternion_c);
	DirectX::XMMATRIX result = DirectX::XMMATRIX(temp_c.r[0], temp_c.r[1], temp_c.r[2], InterpolatePosition(a.r[3], b.r[3], ratio));

	return result;
}

std::vector<Joint*> AnimationInterpolator::InterpolateJoints(const std::vector<Joint*> & a, const std::vector<Joint*> & b, float ratio)
{
	std::vector<Joint*> return_list;
	for (size_t i = 0; i < a.size(); i++)
	{
		Joint* temp_joint = new Joint();
		temp_joint->name = a[i]->name;
		temp_joint->parent_index = a[i]->parent_index;
		temp_joint->matrix = InterpolateMatrix(a[i]->matrix, b[i]->matrix, ratio);
		return_list.push_back(temp_joint);
	}
	return return_list;
}

AnimationInterpolator::AnimationInterpolator()
{
	m_fCurrentTime = 0.0f;
	m_fDuration = 0.0f;
}

AnimationInterpolator::AnimationInterpolator(const Animation * pAnimation)
{
	if (pAnimation)
	{
		m_fCurrentTime = 0.0f;
		m_fDuration = pAnimation->duration;
		m_pAnimation = const_cast<Animation*>(pAnimation);
	}
	else
	{
		m_fCurrentTime = 0.0f;
		m_fDuration = 0.0f;
		m_pAnimation = nullptr;
	}
}

AnimationInterpolator::~AnimationInterpolator()
{
	m_fCurrentTime = 0.0f;
	m_fDuration = 0.0f;
	m_pAnimation = nullptr;
}

AnimationInterpolator::AnimationInterpolator(const AnimationInterpolator & copy)
{
	m_fCurrentTime = copy.m_fCurrentTime;
	m_fDuration = copy.m_fDuration;
	m_pAnimation = copy.m_pAnimation;
}

AnimationInterpolator & AnimationInterpolator::operator=(const AnimationInterpolator & copy)
{
	// TODO: insert return statement here
	if (this != &copy)
	{
		m_fCurrentTime = copy.m_fCurrentTime;
		m_fDuration = copy.m_fDuration;
		m_pAnimation = copy.m_pAnimation;
	}
	return *this;
}

void AnimationInterpolator::SetAnimation(const Animation * pAnimation)
{
	if (pAnimation)
	{
		m_pAnimation = const_cast<Animation*>(pAnimation);
		SetDuration(m_pAnimation->duration);
	}
	else
	{
		m_pAnimation = nullptr;
		SetDuration(0.0f);
	}
}

std::vector<Joint*> AnimationInterpolator::Interpolate(float delta)
{
	if (!m_pAnimation) 
	{
		return std::vector<Joint*>();
	}

	AddTime(delta);

	unsigned int nPreviousIndex = 0;
	unsigned int nCurrentIndex = 1;
	bool found = false;

	// Find the two keyframes that t falls between
	while (!found)
	{
		if (m_fCurrentTime > m_pAnimation->keyframes[nCurrentIndex]->time)
		{
			nPreviousIndex++;
			nCurrentIndex++;

			if (nCurrentIndex >= m_pAnimation->keyframes.size())
			{
				nCurrentIndex = 0;
				nPreviousIndex = (unsigned int)m_pAnimation->keyframes.size()-1;
				found = true;
			}
		}
		else
		{
			found = true;
		}
	}

	// Map t to a 0-to-1 ratio d
	float tween_delta = 0.0f;
	float denominator = 0.0f;
	float numerator = 0.0f;

	denominator = m_pAnimation->keyframes[nCurrentIndex]->time - m_pAnimation->keyframes[nPreviousIndex]->time;
	numerator = m_fCurrentTime - m_pAnimation->keyframes[nPreviousIndex]->time;

	tween_delta = numerator / denominator;

	return InterpolateJoints(m_pAnimation->keyframes[nPreviousIndex]->joints, m_pAnimation->keyframes[nCurrentIndex]->joints, tween_delta);
}
