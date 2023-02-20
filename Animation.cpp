#include "Animation.h"
#include "Motion.h"
#include "Skeleton.h"
#include "dx11mathutil.h"

void Animation::AddAnimationClips(Motion* motion)
{
	m_animationClips.emplace_back(motion);
}

void Animation::UpdateAnimation(float dt)
{
	if (m_animationClips.size() <= 0)
		return;


	Motion* motion = m_animationClips[m_motionIndex];

	m_motionTime += dt * m_motionSpeed;

	if (m_motionTime > motion->GetDuration(this))
	{
		if (motion->GetIsLoop())
			m_motionTime = 0.0f;
	}

	else if (m_motionTime < 0)
	{
		if (motion->GetIsLoop())
			m_motionTime = motion->GetDuration(this);
	}

	// アニメーション行列計算
	std::vector<DirectX::XMFLOAT4X4> animMat;
	motion->CalcAnimationMatrix(this, animMat);

	// 単位行列
	DirectX::XMFLOAT4X4 identity;
	DX11MtxIdentity(identity);

	// ボーン行列計算
	this->m_skeleton->CalcBonesMatrix(
		animMat,
		this->m_skeleton->GetRootBone()->GetBoneIndex(),
		identity,
		this->m_bonesMatrix);
}

void Animation::UpdateConstantBufferBoneMatrix()
{
	Skeleton::UpdateConstantBufferBoneMatrix(m_bonesMatrix);
}

void Animation::Play()
{
	this->m_motionTime = 0.0f;
}

void Animation::Play(int motionIndex)
{
	this->m_motionIndex = motionIndex;
	Play();
}

Skeleton* Animation::GetSkeleton()
{
	return this->m_skeleton;
}

void Animation::SetSkeleton(Skeleton* skeleton)
{
	this->m_skeleton = skeleton;

	if (skeleton == nullptr) return;

	m_bonesMatrix.clear();
	m_bonesMatrix.resize(skeleton->GetBoneNum());
}

float Animation::GetMotionTime() const
{
	return this->m_motionTime;
}

void Animation::SetMotionTime(float time)
{
	this->m_motionTime = time;
}

int Animation::GetMotionIndex() const
{
	return this->m_motionIndex;
}
void Animation::SetMotionIndex(int index)
{
	this->m_motionIndex = index;
	this->m_motionTime = 0.0f;
}

float Animation::GetMotionSpeed() const
{
	return this->m_motionSpeed;
}

void Animation::SetMotionSpeed(float speed)
{
	this->m_motionSpeed = speed;
}

int Animation::GetMotionNum() const
{
	return this->m_animationClips.size();
}

float Animation::GetBlendParameter() const
{
	return this->m_blendParameter;
}

void Animation::SetBlendParameter(float param)
{
	if (param > 1.0f) param = 1.0f;
	else if (param < 0.0f) param = 0.0f;

	this->m_blendParameter = param;
}