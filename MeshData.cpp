#include "MeshData.h"
#include "Assimpscene.h"
#include <assimp\scene.h>

MeshData::~MeshData()
{
	Unload();
}

void MeshData::Load(AssimpScene* assimpScene)
{
	aiNode* rootNode = assimpScene->GetScene()->mRootNode;
	ProcessNode(rootNode, assimpScene);
}

void MeshData::Unload()
{
	// メッシュの解放
	for (int i = 0; i < m_meshes.size(); i++)
	{
		m_meshes[i].Uninit();
	}
}

void MeshData::Draw(ID3D11DeviceContext* devcon)
{
	for (int i = 0; i < m_meshes.size(); i++)
	{
		// 定数バッファセット処理
		m_meshes[i].Draw(devcon);
	}
}

void MeshData::ProcessNode(aiNode* node, AssimpScene* assimpScene)
{
	// ノード内のメッシュの数分ループする
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		int meshindex = node->mMeshes[i];			// ノードのi番目メッシュのインデックスを取得
		aiMesh* mesh = assimpScene->GetScene()->mMeshes[meshindex];	// シーンからメッシュ本体を取り出す

		m_meshes.push_back(this->ProcessMesh(mesh, assimpScene, meshindex));
	}

	// 子ノードについても解析
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		this->ProcessNode(node->mChildren[i], assimpScene);
	}
}

Mesh MeshData::ProcessMesh(aiMesh* mesh, AssimpScene* assimpScene, int meshidx)
{
	std::vector<Vertex> vertices;			// 頂点
	std::vector<unsigned int> indices;		// 面の構成情報
	//std::vector<Texture> textures;			// テクスチャ

	// 頂点情報を取得
	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex;

		vertex.m_Pos.x = mesh->mVertices[i].x;
		vertex.m_Pos.y = mesh->mVertices[i].y;
		vertex.m_Pos.z = mesh->mVertices[i].z;

		// 法線ベクトルが存在するか？
		if (mesh->HasNormals()) {
			vertex.m_Normal.x = mesh->mNormals[i].x;
			vertex.m_Normal.y = mesh->mNormals[i].y;
			vertex.m_Normal.z = mesh->mNormals[i].z;
		}
		else {
			vertex.m_Normal.x = 0.0f;
			vertex.m_Normal.y = 0.0f;
			vertex.m_Normal.z = 0.0f;
		}

		// テクスチャ座標（０番目）が存在するか？
		if (mesh->HasTextureCoords(0)) {
			vertex.m_Tex.x = mesh->mTextureCoords[0][i].x;
			vertex.m_Tex.y = mesh->mTextureCoords[0][i].y;
		}

		vertex.m_BoneNum = 0;

		for (unsigned int b = 0; b < 4; b++)
		{
			vertex.m_BoneIndex[b] = -1;
			vertex.m_BoneWeight[b] = 0.0f;
		}

		vertices.push_back(vertex);
	}

	//ボーンデータ初期化
	for (unsigned int b = 0; b < mesh->mNumBones; b++)
	{
		// メッシュに関連づいてるボーン情報を取得
		aiBone* bone = mesh->mBones[b];

		// ボーンに関連づいている頂点を選び､ウェイト値をセットする
		for (unsigned int widx = 0; widx < bone->mNumWeights; widx++)
		{
			aiVertexWeight weight = bone->mWeights[widx];

			unsigned int vidx = weight.mVertexId;			// このウエイトに関連づいてる頂点idx

			// メッシュの中の何番目か
			vertices[vidx].m_BoneWeight[vertices[vidx].m_BoneNum] = weight.mWeight;

			// 該当するボーン名のインデックス値をセット
			vertices[vidx].m_BoneIndex[vertices[vidx].m_BoneNum] = assimpScene
				->GetBoneIndexByName(bone->mName.C_Str());

			vertices[vidx].m_BoneNum++;

			assert(vertices[vidx].m_BoneNum <= 4);
		}
	}

	//// テクスチャ情報を取得する
	//if (mesh->mMaterialIndex >= 0)
	//{
	//	// このメッシュのマテリアルインデックス値を取得する
	//	int	mtrlidx = mesh->mMaterialIndex;

	//	// シーンからマテリアルデータを取得する
	//	aiMaterial* material = scene->mMaterials[mtrlidx];

	//	// このマテリアルに関連づいたテクスチャを取り出す
	//	std::vector<Texture> diffuseMaps = this->loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse", scene);

	//	// このメッシュで使用しているテクスチャを保存
	//	textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
	//}

	// 面の構成情報を取得
	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];

		for (unsigned int j = 0; j < face.mNumIndices; j++) {
			indices.push_back(face.mIndices[j]);
		}
	}

	return Mesh(vertices, indices);
}