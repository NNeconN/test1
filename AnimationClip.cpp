#include "AnimationClip.h"
#include "Assimpscene.h"
#include <assimp\scene.h>
#include "dx11mathutil.h"
#include "Skeleton.h"
#include "Animation.h"
// アニメーション読み込み
bool AnimationClip::Load(AssimpScene* assimpScene, unsigned int animationIndex)
{
	if (assimpScene == nullptr ||
		animationIndex > assimpScene->GetAnimationsNum())
	{
		m_isLoad = false;
		return false;
	}

	// アニメーション情報取得
	auto animation = assimpScene->GetScene()->mAnimations[animationIndex];

	SetName(animation->mName.C_Str());
	this->m_TicksPerSecond = animation->mTicksPerSecond;
	this->m_keyFrameDuration = animation->mDuration;

	// キーフレーム用配列初期化
	const int boneNum = assimpScene->GetBoneNum();
	ResizeKeyFrames(boneNum);

	// ボーン用のキーフレーム取得
	const int channelNum = animation->mNumChannels;
	for (int i = 0; i < channelNum; i++)
	{
		auto nodeAnim = animation->mChannels[i];
		int boneIndex = assimpScene->GetBoneIndexByName(nodeAnim->mNodeName.C_Str());

		// ポジションのキーフレーム情報取得
		InitPositionKeyFrame(nodeAnim, boneIndex);

		// 回転のキーフレーム情報取得
		InitRotationKeyFrame(nodeAnim, boneIndex);
	}

	m_isLoad = true;
	return true;
}

void AnimationClip::CalcAnimationInterpolationInfo(AnimationInterpolationInfo& output, float currentTime)
{
	output.keyIndex1 = (int)(currentTime * m_TicksPerSecond) + this->m_beginFrameOffset;
	output.keyIndex2 = (output.keyIndex1 + 1) % GetKeyFrameNum();
	output.InterpolationRate = (currentTime * m_TicksPerSecond) - (int)(currentTime * m_TicksPerSecond);
}

void AnimationClip::CalcAnimationTransforms(
	std::vector<BoneTransform>& output,
	Animation* animation,
	float rate)
{
	// アニメーション行列用配列初期化
	const int boneNum = animation->GetSkeleton()->GetBoneNum();
	output.clear();
	output.resize(boneNum);

	// 補間用の情報を取得
	AnimationInterpolationInfo info;
	CalcAnimationInterpolationInfo(info, animation->GetMotionTime() * rate);

	for (int i = 0; i < boneNum; i++)
	{
		KeyFrame& key1 = m_boneKeyFrames[i][info.keyIndex1];
		KeyFrame& key2 = m_boneKeyFrames[i][info.keyIndex2];

		BoneTransform trans1(key1.position, key1.rotation);
		BoneTransform trans2(key2.position, key2.rotation);

		output[i] = BoneTransform::Lerp(trans1, trans2, info.InterpolationRate);
	}
}

bool AnimationClip::GetIsLoad() const
{
	return this->m_isLoad;
}

float AnimationClip::GetKeyFrameDuration() const
{
	return this->m_keyFrameDuration - this->m_beginFrameOffset - this->m_endFrameOffset;
}

float AnimationClip::GetDuration() const
{
	return GetKeyFrameDuration() / m_TicksPerSecond;
}

float AnimationClip::GetDuration(Animation* animation)
{
	return GetDuration();
}

KeyFrame AnimationClip::GetKeyFrame(int boneIndex, int frameIndex) const
{
	return m_boneKeyFrames[boneIndex][frameIndex];
}

int AnimationClip::GetKeyFrameNum(int boneIndex) const
{
	return m_boneKeyFrames[boneIndex].size();
}

int AnimationClip::GetBeginFrameOffset() const
{
	return this->m_beginFrameOffset;
}

void AnimationClip::SetBeginFrameOffset(int offset)
{
	this->m_beginFrameOffset = offset;
}

int AnimationClip::GetEndFrameOffset() const
{
	return m_endFrameOffset;
}

void AnimationClip::SetEndFrameOffset(int offset)
{
	this->m_endFrameOffset = offset;
}

void AnimationClip::ResizeKeyFrames(const int boneNum)
{
	// キーフレーム用配列初期化
	this->m_boneKeyFrames.clear();
	this->m_boneKeyFrames.resize(boneNum);

	// フレーム数分容量を確保
	for (int i = 0; i < boneNum; i++)
	{
		m_boneKeyFrames[i].resize(this->m_keyFrameDuration + 1);

		for (auto& key : m_boneKeyFrames[i])
		{
			key.position = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
			key.rotation = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
			key.shouldLerpPosition = true;
			key.shouldSlerpRotation = true;
		}
	}
}

void AnimationClip::InitPositionKeyFrame(aiNodeAnim* nodeAnim, const int boneIndex)
{
	const int posKeyNum = nodeAnim->mNumPositionKeys;
	for (int keyIndex = 0; keyIndex < posKeyNum; keyIndex++)
	{
		int frameTime = nodeAnim->mPositionKeys[keyIndex].mTime;
		aiVector3D pos = nodeAnim->mPositionKeys[keyIndex].mValue;

		auto& key = m_boneKeyFrames[boneIndex][frameTime];
		key.position = XMFLOAT3(pos.x, pos.y, pos.z);
		key.shouldLerpPosition = false;
	}

	InitLerpPositionKeyFrame(boneIndex);
}

void AnimationClip::InitRotationKeyFrame(aiNodeAnim* nodeAnim, const int boneIndex)
{
	const int rotKeyNum = nodeAnim->mNumRotationKeys;
	for (int keyIndex = 0; keyIndex < rotKeyNum; keyIndex++)
	{
		int frameTime = nodeAnim->mRotationKeys[keyIndex].mTime;
		aiQuaternion rot = nodeAnim->mRotationKeys[keyIndex].mValue;

		auto& key = m_boneKeyFrames[boneIndex][frameTime];
		key.rotation = XMFLOAT4(rot.x, rot.y, rot.z, rot.w);
		key.shouldSlerpRotation = false;
	}

	InitSlerpRotationKeyFrame(boneIndex);
}

void AnimationClip::InitLerpPositionKeyFrame(int boneIndex)
{
	const int keyNum = m_boneKeyFrames[boneIndex].size();
	for (int i = 0; i < keyNum; i++)
	{
		auto& key = m_boneKeyFrames[boneIndex][i];
		if (key.shouldLerpPosition)
		{
			int keyIndex1 = -1;
			int keyIndex2 = -1;

			for (int index = i - 1; index >= 0; index--)
			{
				if (m_boneKeyFrames[boneIndex][index].shouldLerpPosition == false)
				{
					keyIndex1 = index;
					break;
				}
			}

			for (int index = i + 1; index < keyNum; index++)
			{
				if (m_boneKeyFrames[boneIndex][index].shouldLerpPosition == false)
				{
					keyIndex2 = index;
					break;
				}
			}

			// 補間がない場合
			if (keyIndex2 < 0 && keyIndex1 >= 0)
			{
				key.position = m_boneKeyFrames[boneIndex][keyIndex1].position;
			}

			else if (keyIndex1 < 0 && keyIndex2 >= 0)
			{
				key.position = m_boneKeyFrames[boneIndex][keyIndex2].position;
			}

			else if (keyIndex1 >= 0 && keyIndex2 >= 0)
			{
				float rate = (float)(i - keyIndex1) / (float)(keyIndex2 - keyIndex1);
				KeyFrame& key1 = m_boneKeyFrames[boneIndex][keyIndex1];
				KeyFrame& key2 = m_boneKeyFrames[boneIndex][keyIndex2];
				DirectX::XMFLOAT3 pos;

				pos.x = key1.position.x * rate + key2.position.x * (1 - rate);
				pos.y = key1.position.y * rate + key2.position.y * (1 - rate);
				pos.z = key1.position.z * rate + key2.position.z * (1 - rate);
				key.position = pos;
			}
		}
	}
}

void AnimationClip::InitSlerpRotationKeyFrame(int boneIndex)
{
	const int keyNum = m_boneKeyFrames[boneIndex].size();
	for (int i = 0; i < keyNum; i++)
	{
		auto& key = m_boneKeyFrames[boneIndex][i];
		if (key.shouldSlerpRotation)
		{
			int keyIndex1 = -1;
			int keyIndex2 = -1;

			for (int index = i - 1; index >= 0; index--)
			{
				if (m_boneKeyFrames[boneIndex][index].shouldSlerpRotation == false)
				{
					keyIndex1 = index;
					break;
				}
			}

			for (int index = i + 1; index < keyNum; index++)
			{
				if (m_boneKeyFrames[boneIndex][index].shouldSlerpRotation == false)
				{
					keyIndex2 = index;
					break;
				}
			}

			// 補間がない場合
			if (keyIndex2 < 0 && keyIndex1 >= 0)
			{
				key.rotation = m_boneKeyFrames[boneIndex][keyIndex1].rotation;
			}

			else if (keyIndex1 < 0 && keyIndex2 >= 0)
			{
				key.rotation = m_boneKeyFrames[boneIndex][keyIndex2].rotation;
			}

			else if (keyIndex1 >= 0 && keyIndex2 >= 0)
			{
				float rate = (float)(i - keyIndex1) / (float)(keyIndex2 - keyIndex1);
				KeyFrame& key1 = m_boneKeyFrames[boneIndex][keyIndex1];
				KeyFrame& key2 = m_boneKeyFrames[boneIndex][keyIndex2];
				DirectX::XMFLOAT4 rot;

				DX11QtSlerp(
					key1.rotation,
					key2.rotation,
					rate,
					rot);

				key1.rotation = rot;
			}
		}
	}
}