#pragma once
#include <d3d11.h>
#include <string>

class Texture
{
public:
	bool Load(std::string fileName, std::string filepath);
	void SetTexture(int slotIndex);

private:
	ID3D11Resource* m_resource;
	ID3D11ShaderResourceView* m_srv; // SRV
};