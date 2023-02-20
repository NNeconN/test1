#pragma once
#include <string>
#include <vector>
#include <DirectXMath.h>
#include "BoneTransform.h"
#include "KeyFrame.h"

class Skeleton;
class Animation;

// アニメーションの補間用のデータ構造体
struct AnimationInterpolationInfo
{
	int keyIndex1;
	int keyIndex2;
	float InterpolationRate;
};

// モーションの基礎クラス
class Motion
{
public:
	// アニメーション行列を計算
	void CalcAnimationMatrix(
		Animation* animation,
		std::vector<DirectX::XMFLOAT4X4>& outputMatrix);

	virtual float GetDuration(Animation* animation) abstract;

	virtual void CalcAnimationTransforms(
		std::vector<BoneTransform>& output,
		Animation* animation,
		float rate = 1.0f) abstract;

public:
	// ゲッターセッター
	std::string GetName() const;
	void SetName(std::string name);

	bool GetIsLoop() const;

private:
	std::string m_name;
	bool m_isLoop = true;	// ループするかどうか
};

