#include "BlendAnimation.h"
#include "dx11mathutil.h"
#include "Skeleton.h"
#include "Animation.h"

void BlendAnimation::AddBlendSample(AnimationClip* sample, float blendParameter)
{
	BlendSample blendsample;
	blendsample.sample = sample;
	blendsample.blendParameter = blendParameter;

	m_blendSamles.emplace_back(blendsample);
}

float BlendAnimation::GetDuration(Animation* animation)
{
	AnimationBlendInterpolationInfo info;
	CalcBlendInterpolationInfo(info, animation->GetBlendParameter());

	float result;

	const float d1 = info.sample1->GetDuration();
	const float d2 = info.sample2->GetDuration();
	const float rate = info.interpolationRate;

	result = d1 * (1 - rate) + rate * d2;

	return result;
}

void BlendAnimation::CalcBlendInterpolationInfo(
	AnimationBlendInterpolationInfo& output,
	float m_blendWeightParam)
{
	const int sampleNum = m_blendSamles.size();
	if (sampleNum == 0)
	{
		output.sample1 = nullptr;
		output.sample2 = nullptr;
		output.interpolationRate = 0.0f;
		return;
	}

	float low = -100000.0f;
	float high = 100000.0f;
	for (int i = 0; i < sampleNum; i++)
	{
		float diff = m_blendSamles[i].blendParameter - m_blendWeightParam;

		if (diff <= 0 && diff > low)
		{
			output.sample1 = m_blendSamles[i].sample;
			low = diff;
		}

		if (diff >= 0 && diff < high)
		{
			output.sample2 = m_blendSamles[i].sample;
			high = diff;
		}
	}

	if (abs(high - low) > FLT_EPSILON)
		output.interpolationRate = -low / (high - low);
	else
		output.interpolationRate = 0.0f;
}

void BlendAnimation::CalcAnimationTransforms(
	std::vector<BoneTransform>& output,
	Animation* animation,
	float rate)
{
	// アニメーション行列用配列初期化
	const int boneNum = animation->GetSkeleton()->GetBoneNum();
	output.clear();
	output.resize(boneNum);

	// 補間すべきアニメーションを計算して求める
	AnimationBlendInterpolationInfo info;
	CalcBlendInterpolationInfo(info, animation->GetBlendParameter());

	std::vector<BoneTransform> boneTransforms1, boneTransforms2;

	float duration = GetDuration(animation);

	float rate1 = info.sample1->GetDuration() / duration;
	float rate2 = info.sample2->GetDuration() / duration;


	// 補間する二つのアニメーションの姿勢を求める
	info.sample1->CalcAnimationTransforms(boneTransforms1, animation, rate1);
	info.sample2->CalcAnimationTransforms(boneTransforms2, animation, rate2);

	// ブレンドする
	for (int i = 0; i < boneNum; i++)
	{
		output[i] = BoneTransform::Lerp(
			boneTransforms1[i],
			boneTransforms2[i],
			info.interpolationRate);
	}
}