#include "Mesh.h"
#include "Shader.h"
#include "CDirectxGraphics.h"
Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices)
{
	this->m_vertices = vertices;
	this->m_indices = indices;

	InitBuffers();
}

void Mesh::Uninit()
{
	if (this->m_VertexBuffer == nullptr)
	{
		this->m_VertexBuffer->Release();
		this->m_VertexBuffer = nullptr;
	}

	if (this->m_IndexBuffer == nullptr)
	{
		this->m_IndexBuffer->Release();
		this->m_IndexBuffer = nullptr;
	}
}

void Mesh::Draw(ID3D11DeviceContext* devcon)
{
	unsigned int stride = sizeof(Vertex);
	unsigned int offset = 0;
	// 頂点バッファをセット
	devcon->IASetVertexBuffers(0, 1, &m_VertexBuffer, &stride, &offset);
	// インデックスバッファをセット
	devcon->IASetIndexBuffer(m_IndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	// トポロジーをセット
	devcon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// インデックスバッファを利用して描画
	devcon->DrawIndexed(static_cast<unsigned int>(m_indices.size()), 0, 0);
}

bool Mesh::InitBuffers()
{
	ID3D11Device* dev;
	dev = CDirectXGraphics::GetInstance()->GetDXDevice();

	// 頂点バッファ生成
	bool sts = CreateVertexBufferWrite(dev,
		static_cast<unsigned int>(sizeof(Vertex)),			// ストライド
		static_cast<unsigned int>(this->m_vertices.size()),		// 頂点数
		this->m_vertices.data(),									// 頂点データ
		&this->m_VertexBuffer);
	if (!sts) {
		return false;
	}

	// インデックスバッファ生成
	sts = CreateIndexBuffer(dev,
		static_cast<unsigned int>(this->m_indices.size()),
		this->m_indices.data(),
		&this->m_IndexBuffer);
	if (!sts) {
		return false;
	}
	return true;
}