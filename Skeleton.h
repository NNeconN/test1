#pragma once
#include "Bone.h"
#include <d3d11.h>
struct aiNode;

// スケルトン（骨の集合体）
class Skeleton
{
public:
	bool Load(AssimpScene* assimpScene);

	void CalcBonesMatrix(
		const std::vector<DirectX::XMFLOAT4X4>& animationMatrix,
		int index,
		DirectX::XMFLOAT4X4 parentMatrix,
		std::vector<DirectX::XMFLOAT4X4>& outputMatrix);

public:
	// ゲッターセッター
	Bone* GetRootBone();
	Bone* GetBoneByIndex(int index);
	int GetBoneNum() const;

public:
	// 所持できるボーンの最大数
	static constexpr unsigned int MAX_BONE_NUM = 400;

public:
	static void InitConstantBufferBoneMatrix(ID3D11Device* device);
	static void UninitConstantBufferBoneMatrix();
	static void UpdateConstantBufferBoneMatrix(const std::vector<DirectX::XMFLOAT4X4>& matrix);

private:

	// ボーンを生成
	void CreateBones(AssimpScene* assimpScene, aiNode* node, int parentIndex);

	// ボーンの親子関係を形成
	void CreateBoneTree();

	// ボーンオフセット行列初期化
	void InitBonesOffsetMatrix(AssimpScene* assimpScene);

private:
	std::vector<Bone> m_bones;	// ボーン配列
	Bone* m_rootBone;			// ルートとなるボーン
	bool m_isLoad = false;		// 初期化したかどうか

private:
	static ID3D11Buffer* m_constantbufferbonematrix;	// ボーン行列格納用定数バッファ
};

// ボーン行列格納用のコンスタントバッファ
struct ConstantBufferBoneMatrix
{
	DirectX::XMFLOAT4X4  mBoneMatrix[Skeleton::MAX_BONE_NUM];
};