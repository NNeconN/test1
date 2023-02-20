#pragma once
#include <string>
#include <unordered_map>

struct aiScene;
struct aiNode;

class AssimpScene
{
public:
	// 初期処理
	bool Init(std::string filename);
	// 終了処理
	void Exit();

	// シーン取得
	const aiScene* GetScene() const;

	// アニメーションを持っているか
	bool HasAnimation();

	// アニメーションの数
	unsigned int GetAnimationsNum() const;

	// 名前からボーンのインデックスを取得する
	int GetBoneIndexByName(std::string name) const;

	// ボーンの数を取得
	unsigned int GetBoneNum() const;

private:
	// ボーンの名前とインデックスを関連付けるマップ作成
	void CreateBoneIndexMap(aiNode* node);

private:
	const aiScene* m_Scene = nullptr;	// assimp scene

	// ボーンの名前とインデックスを関連付ける
	std::unordered_map<std::string, int> m_boneIndexMap;
};