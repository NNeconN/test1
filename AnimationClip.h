#pragma once
#include "Motion.h"

class AssimpScene;
class aiNodeAnim;

//　アニメーションクリップ
class AnimationClip : public Motion
{
	friend class BlendAnimation;

public:
	// アニメーション読み込み
	bool Load(AssimpScene* assimpScene, unsigned int animationIndex);

	virtual float GetDuration(Animation* animation) override;
	float GetDuration() const;

	// アニメーションの姿勢を求める
	virtual void CalcAnimationTransforms(
		std::vector<BoneTransform>& output,
		Animation* animationo,
		float rate) override;

public:
	bool GetIsLoad() const;
	float GetKeyFrameDuration() const;
	KeyFrame GetKeyFrame(int boneIndex, int frameIndex) const;
	int GetKeyFrameNum(int boneIndex = 0) const;

	int GetBeginFrameOffset() const;
	void SetBeginFrameOffset(int offset);
	int GetEndFrameOffset() const;
	void SetEndFrameOffset(int offset);

private:
	void ResizeKeyFrames(const int boneNum); // キーフレーム配列のメモリを確保
	void InitPositionKeyFrame(aiNodeAnim* nodeAnim, const int boneIndex);
	void InitRotationKeyFrame(aiNodeAnim* nodeAnim, const int boneIndex);
	void InitLerpPositionKeyFrame(int boneIndex);
	void InitSlerpRotationKeyFrame(int boneIndex);

	// アニメーションの補間用の情報取得
	void CalcAnimationInterpolationInfo(
		AnimationInterpolationInfo& output,
		float currentTime);

private:
	bool m_isLoad = false;		// 初期化したかどうか
	float m_keyFrameDuration;	// アニメーションの時間
	float m_TicksPerSecond;		// アニメーションの1フレームの時間
	int m_beginFrameOffset = 0; // 始まりのフレームのオフセット
	int m_endFrameOffset = 0;	// 終わりのフレームのオフセット
	std::vector<std::vector<KeyFrame>> m_boneKeyFrames;  // キーフレーム情報
};

