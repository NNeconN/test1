#pragma once
#include <d3d11.h>

class Material
{
public:
	~Material();

public:
	bool LoadShader(const char* vsfile, const char* psfile);
	void Unload();

	// シェーダをセットする
	void SetShader();

private:
	ID3D11VertexShader* m_pVertexShader = nullptr;		// 頂点シェーダー入れ物
	ID3D11PixelShader* m_pPixelShader = nullptr;		// ピクセルシェーダー入れ物
	ID3D11InputLayout* m_pVertexLayout = nullptr;		// 頂点フォーマット定義
};

