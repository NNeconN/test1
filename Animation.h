#pragma once
#include <vector>
#include <DirectXMath.h>
class Motion;
class Skeleton;

// アニメーションを再生するクラス
class Animation
{
public:
	// モーション配列に追加
	void AddAnimationClips(Motion* motion);

	// アニメーション更新
	void UpdateAnimation(float dt);

	// 定数バッファ更新
	void UpdateConstantBufferBoneMatrix();

	// アニメーションを初めから再生
	void Play();
	void Play(int motionIndex);

public:
	// ゲッターセッター
	Skeleton* GetSkeleton();
	void SetSkeleton(Skeleton* skeleton);

	float GetMotionTime() const;
	void SetMotionTime(float time);

	int GetMotionIndex() const;
	void SetMotionIndex(int index);

	float GetMotionSpeed() const;
	void SetMotionSpeed(float speed);

	int GetMotionNum() const;

	float GetBlendParameter() const;
	void SetBlendParameter(float param);

private:
	std::vector<Motion*> m_animationClips;          // モーションの配列
	std::vector<DirectX::XMFLOAT4X4> m_bonesMatrix; // 定数バッファに適用するボーン情報
	Skeleton* m_skeleton;

private:
	float m_motionSpeed = 1.0f; // 再生スピード
	float m_motionTime = 0.0f;  // 現在再生中の時間
	int m_motionIndex = 0;     // 現在再生中のモーション番号
	float m_blendParameter = 0.0f; // ブレンドツリー用のパラメータ
};

