#pragma once
#include <DirectXMath.h>

class BoneTransform
{
public:
	BoneTransform();
	BoneTransform(DirectX::XMFLOAT3 pos, DirectX::XMFLOAT4 rot);

public:
	// ゲッターセッター
	DirectX::XMFLOAT3 GetPosition() const;
	void SetPosition(DirectX::XMFLOAT3 pos);

	DirectX::XMFLOAT4 GetRotation() const;
	void SetRotation(DirectX::XMFLOAT4 rot);

	DirectX::XMFLOAT4X4 GetMatrix() const;

public:
	static BoneTransform Lerp(const BoneTransform& trans1,
		const BoneTransform& trans2,
		float rate);

private:
	DirectX::XMFLOAT3 m_position;
	DirectX::XMFLOAT4 m_rotation;
};

