#include "Skeleton.h"
#include "Assimpscene.h"
#include <assimp\scene.h>
#include "dx11mathutil.h"
#include "Shader.h"
#include "CDirectxGraphics.h"
ID3D11Buffer* Skeleton::m_constantbufferbonematrix;

void Skeleton::InitConstantBufferBoneMatrix(ID3D11Device* device)
{
	bool sts = CreateConstantBufferWrite(device, sizeof(ConstantBufferBoneMatrix), &m_constantbufferbonematrix);
	if (!sts) {
		MessageBox(nullptr, "constant buffer create(bonematrix) fail", "error", MB_OK);
	}
}

void Skeleton::UninitConstantBufferBoneMatrix()
{
	if (m_constantbufferbonematrix != nullptr)
	{
		m_constantbufferbonematrix->Release();
		m_constantbufferbonematrix = nullptr;
	}
}

void Skeleton::UpdateConstantBufferBoneMatrix(const std::vector<DirectX::XMFLOAT4X4>& matrix)
{
	ConstantBufferBoneMatrix cb;

	const int size = matrix.size();

	//初期化
	for (int i = 0; i < MAX_BONE_NUM; i++)
	{
		DX11MtxIdentity(cb.mBoneMatrix[i]);
	}

	for (int i = 0; i < size; i++)
	{
		DX11MtxTranspose(cb.mBoneMatrix[i], matrix[i]);
	}

	ID3D11DeviceContext* devicecontext;

	devicecontext = CDirectXGraphics::GetInstance()->GetImmediateContext();

	D3D11_MAPPED_SUBRESOURCE pData;

	// ボーン行列格納用定数バッファ更新
	HRESULT hr = devicecontext->Map(m_constantbufferbonematrix, 0, D3D11_MAP_WRITE_DISCARD, 0, &pData);
	if (SUCCEEDED(hr)) {
		memcpy_s(pData.pData, pData.RowPitch, (void*)&cb, sizeof(ConstantBufferBoneMatrix));
		devicecontext->Unmap(m_constantbufferbonematrix, 0);
	}

	// GPUへ定数バッファをセット
	devicecontext->VSSetConstantBuffers(5, 1, &m_constantbufferbonematrix);
}

bool Skeleton::Load(AssimpScene* assimpScene)
{
	if (assimpScene == nullptr)
	{
		this->m_isLoad = false;
		return false;
	}

	// ボーン配列用のメモリを確保
	this->m_bones.clear();
	this->m_bones.resize(assimpScene->GetBoneNum());

	// ボーン生成
	CreateBones(assimpScene,
		assimpScene->GetScene()->mRootNode,
		Bone::NONE_PARENT);

	// ボーンの親子関係を形成
	CreateBoneTree();

	// ボーンオフセット行列初期化
	InitBonesOffsetMatrix(assimpScene);

	this->m_isLoad = true;
	return true;
}

Bone* Skeleton::GetRootBone()
{
	return this->m_rootBone;
}

Bone* Skeleton::GetBoneByIndex(int index)
{
	return &m_bones[index];
}

int Skeleton::GetBoneNum() const
{
	return m_bones.size();
}

// ボーンを生成
void Skeleton::CreateBones(AssimpScene* assimpScene, aiNode* node, int parentIndex)
{
	// 名前からボーン配列のインデックス番号を取得
	int boneIndex = assimpScene->GetBoneIndexByName(node->mName.C_Str());

	// ボーン情報を保存する
	Bone& bone = m_bones[boneIndex];
	bone.SetParentIndex(parentIndex);
	bone.SetName(node->mName.C_Str());
	bone.SetBoneIndex(boneIndex);

	const unsigned int childCount = node->mNumChildren;
	for (int i = 0; i < childCount; i++)
	{
		aiNode* child = node->mChildren[i];
		CreateBones(assimpScene, child, boneIndex);
	}
}

// ボーンの親子関係を形成
void Skeleton::CreateBoneTree()
{
	// ルートボーンを設定
	this->m_rootBone = &this->m_bones[0];

	const int boneNum = m_bones.size();
	for (int i = 0; i < boneNum; i++)
	{
		Bone* bone = &this->m_bones[i];

		// 親がいれば自分を子にする
		const int parentIndex = this->m_bones[i].GetParentIndex();
		if (parentIndex != Bone::NONE_PARENT)
		{
			this->m_bones[parentIndex].AddChild(bone);
		}
	}
}

void Skeleton::InitBonesOffsetMatrix(AssimpScene* assimpScene)
{
	// メッシュ情報取得の中にボーン情報がまぎれている

	const int meshNum = assimpScene->GetScene()->mNumMeshes;
	for (int meshIndex = 0; meshIndex < meshNum; meshIndex++)
	{
		auto mesh = assimpScene->GetScene()->mMeshes[meshIndex];

		const int boneNum = mesh->mNumBones;
		for (int boneIndex = 0; boneIndex < boneNum; boneIndex++)
		{
			auto bone = mesh->mBones[boneIndex];
			int idx = assimpScene->GetBoneIndexByName(bone->mName.C_Str());

			DirectX::XMFLOAT4X4 offset = DX11MtxaiToDX(bone->mOffsetMatrix);
			m_bones[idx].SetOffsetMatrix(offset);
		}
	}
}

void Skeleton::CalcBonesMatrix(
	const std::vector<DirectX::XMFLOAT4X4>& animationMatrix,
	int index,
	DirectX::XMFLOAT4X4 parentMatrix,
	std::vector<DirectX::XMFLOAT4X4>& outputMatrix)
{
	DirectX::XMFLOAT4X4 worldMatrix;
	DX11MtxMultiply(
		worldMatrix,
		animationMatrix[index],
		parentMatrix);

	DirectX::XMFLOAT4X4 offsetMatrix;
	offsetMatrix = m_bones[index].GetOffsetMatrix();

	DirectX::XMFLOAT4X4 boneMatrix;
	DX11MtxMultiply(
		boneMatrix,
		offsetMatrix,
		worldMatrix);

	outputMatrix[index] = boneMatrix;

	const int childNum = m_bones[index].GetChildCount();
	for (int i = 0; i < childNum; i++)
	{
		Bone* child = m_bones[index].GetChild(i);
		CalcBonesMatrix(
			animationMatrix,
			child->GetBoneIndex(),
			worldMatrix,
			outputMatrix);
	}
}