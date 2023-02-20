#pragma once
#include "Mesh.h"

class AssimpScene;
struct aiNode;
struct aiMesh;

// メッシュのデータクラス
class MeshData
{
public:
	~MeshData();

public:
	void Load(AssimpScene* assimpScene); // 読み込み
	void Unload();

	void Draw(ID3D11DeviceContext* devcon);

private:
	// ノード解析
	void ProcessNode(aiNode* node, AssimpScene* assimpScene);

	// メッシュ解析
	Mesh ProcessMesh(aiMesh* mesh, AssimpScene* assimpScene, int meshidx);

private:
	std::vector<Mesh> m_meshes; // メッシュ配列
};

