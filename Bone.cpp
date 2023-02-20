#include "Bone.h"
#include "dx11mathutil.h"
Bone::Bone()
{
	DX11MtxIdentity(m_offsetMatrix);
}

void Bone::AddChild(Bone* bone)
{
	this->m_children.emplace_back(bone);
}

unsigned int Bone::GetChildCount() const
{
	return this->m_children.size();
}

Bone* Bone::GetChild(int index)
{
	return this->m_children[index];
}

int Bone::GetBoneIndex() const
{
	return this->m_boneIndex;
}

void Bone::SetBoneIndex(int index)
{
	this->m_boneIndex = index;
}

int Bone::GetParentIndex() const
{
	return this->m_parentIndex;
}

void Bone::SetParentIndex(int index)
{
	this->m_parentIndex = index;
}

std::string Bone::GetName() const
{
	return this->m_name;
}

void Bone::SetName(std::string name)
{
	this->m_name = name;
}

DirectX::XMFLOAT4X4 Bone::GetOffsetMatrix() const
{
	return this->m_offsetMatrix;
}

void Bone::SetOffsetMatrix(DirectX::XMFLOAT4X4 matrix)
{
	this->m_offsetMatrix = matrix;
}