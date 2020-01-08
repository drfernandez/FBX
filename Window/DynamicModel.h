#pragma once

#include "StaticModel.h"
#include "AnimationInterpolator.h"

class DynamicModel : public StaticModel
{
private:
	std::vector<Joint*>			m_BindPose;
	std::vector<Animation*>		m_Animations;
	AnimationInterpolator		m_Interpolator;
	std::vector<Joint*>			m_CurrentPose;

	bool LoadBindPose(const std::string& pFilename, std::vector<Joint*>& bindpose);
	bool LoadAnimations(const std::string& pFilename, std::vector<Animation*>& animations);

public:
	DynamicModel(void);
	DynamicModel(BinaryReader* reader, VertexBufferManager* vbm, IndexBufferManager* ibm, TextureManager* tm);
	~DynamicModel(void);
	DynamicModel(const DynamicModel& copy) = delete;
	DynamicModel& operator=(const DynamicModel& copy) = delete;

	virtual void LoadModelFromFile(std::string szMeshFilename, 
		std::string szBindposeFilename,
		std::vector<std::string> szAnimationFilenames);

	inline void SetBindPose(const std::vector<Joint*>& bindpose) { m_BindPose = bindpose; }
	inline void SetAnimations(const std::vector<Animation*>& animations) { m_Animations = animations; }

	inline std::vector<Joint*>& GetBindPose(void) { return m_BindPose; }
	inline const std::vector<Joint*>& GetBindPose(void) const { return m_BindPose; }
	inline std::vector<Animation*>& GetAnimations(void) { return m_Animations; }
	inline const std::vector<Animation*>& GetAnimations(void) const { return m_Animations; }
	inline AnimationInterpolator& GetAnimationInterpolator(void) { return m_Interpolator; }
	inline const AnimationInterpolator& GetAnimationInterpolator(void) const { return m_Interpolator; }
	inline std::vector<Joint*>& GetCurrentPose(void) { return m_CurrentPose; }
	inline const std::vector<Joint*>& GetCurrentPose(void) const { return m_CurrentPose; }
};