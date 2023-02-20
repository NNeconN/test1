#pragma once
#include "AnimationClip.h"

class Animation;

// ブレンドするアニメーション
struct BlendSample
{
	AnimationClip* sample;      // モーション
	float blendParameter;// ブレンド用
};

// ブレンド用の情報
struct AnimationBlendInterpolationInfo
{
	AnimationClip* sample1;
	AnimationClip* sample2;
	float interpolationRate;
};

// ブレンドアニメーション
class BlendAnimation : public Motion
{
public:
	void AddBlendSample(AnimationClip* sample, float blendParameter = 0.0f);

public:
	float virtual GetDuration(Animation* animation) override;

	// アニメーションの姿勢を求める
	virtual void CalcAnimationTransforms(
		std::vector<BoneTransform>& output,
		Animation* animationo,
		float rate = 1.0f) override;

public:
	// ブレンド情報を計算する
	void CalcBlendInterpolationInfo(
		AnimationBlendInterpolationInfo& output,
		float m_blendWeightParam);

private:
	std::vector<BlendSample> m_blendSamles;
};

