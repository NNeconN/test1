#pragma once
#include	<d3d11.h>
#include	<directxmath.h>
#include	<vector>

struct Vertex
{
	DirectX::XMFLOAT3 m_Pos;	// 位置
	DirectX::XMFLOAT3 m_Normal;	// 法線
	DirectX::XMFLOAT2 m_Tex;	// UV座標
	int32_t	m_BoneIndex[4];	//ボーンインデックス
	float	m_BoneWeight[4];// ウェイト値
	int32_t	m_BoneNum;		// ボーン数
};

class Mesh
{
public:
	Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices);

public:
	void Uninit(); // 後処理
	void Draw(ID3D11DeviceContext* devcon);

private:
	bool InitBuffers(); // バッファ初期化

private:
	std::vector<Vertex> m_vertices;			// 頂点データ
	std::vector<unsigned int> m_indices;

private:
	ID3D11Buffer* m_VertexBuffer;			// 頂点バッファ
	ID3D11Buffer* m_IndexBuffer;
};