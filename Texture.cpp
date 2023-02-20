#include "Texture.h"
#include "Shader.h"
#include "CDirectxGraphics.h"

bool Texture::Load(std::string fileName, std::string filepath)
{
	std::string name = filepath + "/" + fileName;

	bool sts = CreatetSRVfromFile(
		name.c_str(),
		CDirectXGraphics::GetInstance()->GetDXDevice(),
		CDirectXGraphics::GetInstance()->GetImmediateContext(),
		&this->m_srv);

	if (!sts) {
		MessageBox(nullptr, (name + "load error").c_str(), "error", MB_OK);
		return false;
	}

	return true;
}

void Texture::SetTexture(int slotIndex)
{
	CDirectXGraphics::GetInstance()->GetImmediateContext()
		->PSSetShaderResources(slotIndex, 1, &this->m_srv);
}