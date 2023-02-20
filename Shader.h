#pragma once
#include	<d3dcompiler.h>
#include	<directxmath.h>
#include	<locale.h>
#include	<d3d11.h>
#include	<DirectXMath.h>
#include	<memory>
#include	"WICTextureLoader.h"
#include	"DDSTextureLoader.h"
#include	"directXtex.h"
#include	<string>
using namespace DirectX;

HRESULT CompileShaderFromFile(
	const char* szFileName,
	LPCSTR szEntryPoint,
	LPCSTR szShaderModel,
	ID3DBlob** ppBlobOut);

bool CreateConstantBuffer(
	ID3D11Device* device,
	unsigned int bytesize,
	ID3D11Buffer** pConstantBuffer			// �R���X�^���g�o�b�t�@
);

bool CreateConstantBufferWrite(
	ID3D11Device* device,
	unsigned int bytesize,
	ID3D11Buffer** pConstantBuffer			// �R���X�^���g�o�b�t�@
);


bool CreateIndexBuffer(
	ID3D11Device* device,
	unsigned int indexnum,	// ���_��
	void* indexdata,							// �C���f�b�N�X�f�[�^�i�[�������擪�A�h���X
	ID3D11Buffer** pIndexBuffer);

bool CreateVertexBuffer(
	ID3D11Device* device,
	unsigned int stride,				// �P���_������o�C�g��
	unsigned int vertexnum,				// ���_��
	void* vertexdata,										// ���_�f�[�^�i�[�������擪�A�h���X
	ID3D11Buffer** pVertexBuffer							// ���_�o�b�t�@
);

bool CreateVertexBufferWrite(
	ID3D11Device* device,
	unsigned int stride,				// �P���_������o�C�g��
	unsigned int vertexnum,				// ���_��
	void* vertexdata,					// ���_�f�[�^�i�[�������擪�A�h���X
	ID3D11Buffer** pVertexBuffer		// ���_�o�b�t�@
);

bool CreateVertexBufferUAV(
	ID3D11Device* device,
	unsigned int stride,				// �P���_������o�C�g��
	unsigned int vertexnum,				// ���_��
	void* vertexdata,					// ���_�f�[�^�i�[�������擪�A�h���X
	ID3D11Buffer** pVertexBuffer		// ���_�o�b�t�@
);

bool CreateStructuredBuffer(
	ID3D11Device* device,
	unsigned int stride,				// �X�g���C�h�o�C�g��
	unsigned int num,					// ��
	void* data,							// �f�[�^�i�[�������擪�A�h���X
	ID3D11Buffer** pStructuredBuffer	// StructuredBuffer
);

ID3D11Buffer* CreateAndCopyToBuffer(
	ID3D11Device* device,
	ID3D11DeviceContext* devicecontext,
	ID3D11Buffer* pBuffer	// RWStructuredBuffer
);

bool CreateShaderResourceView(			// �o�b�t�@����V�F�[�_�[���\�[�X�r���[���쐬����	
	ID3D11Device* device,
	ID3D11Buffer* pBuffer,	// Buffer
	ID3D11ShaderResourceView** ppSRV);

bool CreateUnOrderAccessView(
	ID3D11Device* device,
	ID3D11Buffer* pBuffer,	// Buffer
	ID3D11UnorderedAccessView** ppUAV);

bool CreatetSRVfromTGAFile(const char* filename,					// TGA�t�@�C������V�F�[�_�[���\�[�X�r���[���쐬����
	ID3D11Device* device,
	ID3D11ShaderResourceView** srv);

bool CreatetSRVfromWICFile(const char* filename,					// WIC�t�@�C������V�F�[�_�[���\�[�X�r���[���쐬����
	ID3D11Device* device,
	ID3D11DeviceContext* device11Context,
	ID3D11ShaderResourceView** srv);

bool CreateVertexShader(ID3D11Device* device,						// ���_�V�F�[�_�[�I�u�W�F�N�g�𐶐��A�����ɒ��_���C�A�E�g������
	const char* szFileName,
	LPCSTR szEntryPoint,
	LPCSTR szShaderModel,
	D3D11_INPUT_ELEMENT_DESC* layout,
	unsigned int numElements,
	ID3D11VertexShader** ppVertexShader,
	ID3D11InputLayout** ppVertexLayout);


bool CreatePixelShader(ID3D11Device* device,						// �s�N�Z���V�F�[�_�[�I�u�W�F�N�g�𐶐�
	const char* szFileName,
	LPCSTR szEntryPoint,
	LPCSTR szShaderModel,
	ID3D11PixelShader** ppPixelShader);

bool CreatetSRVfromDDS(const char* filename,
	ID3D11Device* device,
	ID3D11DeviceContext* device11Context,
	ID3D11ShaderResourceView** srv);

bool CreatetSRVfromFile(
	const char* filename,
	ID3D11Device* device,
	ID3D11DeviceContext* device11Context,
	ID3D11ShaderResourceView** srv);

std::string ExtractExtension(std::string fullpath);
std::string ExtractFileName(std::string fullpath, char split);