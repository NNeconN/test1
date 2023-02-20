#include "BoneTransform.h"
#include "dx11mathutil.h"

BoneTransform::BoneTransform()
{
	this->m_position = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	this->m_rotation = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
}

BoneTransform::BoneTransform(DirectX::XMFLOAT3 pos, DirectX::XMFLOAT4 rot)
{
	this->m_position = pos;
	this->m_rotation = rot;
}

DirectX::XMFLOAT3 BoneTransform::GetPosition() const
{
	return this->m_position;
}

void BoneTransform::SetPosition(DirectX::XMFLOAT3 pos)
{
	this->m_position = pos;
}

DirectX::XMFLOAT4 BoneTransform::GetRotation() const
{
	return this->m_rotation;
}

void BoneTransform::SetRotation(DirectX::XMFLOAT4 rot)
{
	this->m_rotation = rot;
}

DirectX::XMFLOAT4X4 BoneTransform::GetMatrix() const
{
	DirectX::XMFLOAT4X4 translateMat;
	DX11MtxTranslation(m_position, translateMat);

	DirectX::XMFLOAT4X4 rotMat;
	DX11MtxFromQt(rotMat, m_rotation);

	DirectX::XMFLOAT4X4 result;
	DX11MtxMultiply(result, rotMat, translateMat);

	return result;
}

BoneTransform BoneTransform::Lerp(const BoneTransform& trans1,
	const BoneTransform& trans2,
	float rate)
{
	BoneTransform result;

	// ƒ|ƒWƒVƒ‡ƒ“‚Ì•âŠÔ‚ğ‹‚ß‚é
	result.m_position.x = trans1.m_position.x * rate + trans2.m_position.x * (1 - rate);
	result.m_position.y = trans1.m_position.y * rate + trans2.m_position.y * (1 - rate);
	result.m_position.z = trans1.m_position.z * rate + trans2.m_position.z * (1 - rate);

	// ‰ñ“]‚Ì•âŠÔ‚ğ‹‚ß‚é
	DX11QtSlerp(
		trans1.m_rotation,
		trans2.m_rotation,
		rate,
		result.m_rotation
	);

	return result;
}