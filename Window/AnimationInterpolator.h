#pragma once

#include "header.h"


class AnimationInterpolator
{
private:
	float			m_fCurrentTime;
	float			m_fDuration;

	Animation*		m_pAnimation;


	void AddTime(float t);
	void SetTime(float t);
	void SetDuration(float t);
	float LinearInterpolation(const float& a, const float& b, float ratio);
	DirectX::XMVECTOR InterpolatePosition(const DirectX::XMVECTOR& a, const DirectX::XMVECTOR& b, float ratio);
	DirectX::XMMATRIX InterpolateMatrix(const DirectX::XMMATRIX& a, const DirectX::XMMATRIX& b, float ratio);
	std::vector<Joint*> InterpolateJoints(const std::vector<Joint*>& a, const std::vector<Joint*>& b, float ratio);

public:
	AnimationInterpolator();
	AnimationInterpolator(const Animation* pAnimation);
	~AnimationInterpolator();
	AnimationInterpolator(const AnimationInterpolator& copy);
	AnimationInterpolator& operator=(const AnimationInterpolator& copy);

	void SetAnimation(const Animation* pAnimation);

	std::vector<Joint*> Interpolate(float delta);

	inline float GetTimeCurrent(void) { return m_fCurrentTime; }
	inline float GetTimeDuration(void) { return m_fDuration; }

	inline void SetTimeCurrent(const float& time) { m_fCurrentTime = time; }
	inline void SetTimeDuration(const float& time) { m_fDuration = time; }

};