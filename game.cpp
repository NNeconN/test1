//*****************************************************************************
//!	@file	game.cpp
//!	@brief	
//!	@note	�Q�[������
//!	@author	
//*****************************************************************************

//-----------------------------------------------------------------------------
//	Include header files.
//-----------------------------------------------------------------------------
#define NOMINMAX
#include <windows.h>
#include <stdio.h>
#include "game.h"
#include "CDirectxGraphics.h"
#include "DX11Settransform.h"
#include "CCamera.h"
#include "dx11mathutil.h"
#include "CDirectInput.h"
#include "CLight.h"
#include "Assimpscene.h"
#include "Skeleton.h"
#include "AnimationClip.h"
#include "Animation.h"
#include "BlendAnimation.h"
#include "MeshData.h"
#include "Material.h"
#include "Texture.h"
#include "AnimationData.h"//test
#include <memory>

//-----------------------------------------------------------------------------
// �O���[�o���ϐ�
//-----------------------------------------------------------------------------
//CModel			g_model;			// ��l�����f��
XMFLOAT4X4		g_mtxplayer;		// ��l���̍s��

CLight			g_directionallight;	// ���s����

MeshData		g_meshData;			// ���b�V���̃f�[�^
Skeleton		g_skeleton;			// ���f���̃X�P���g��
std::vector<std::unique_ptr<AnimationClip>> g_animationClips; // �ǂݍ��񂾃A�j���[�V�����N���b�v�̔z��
Animation		g_animation;		// �A�j���[�V����������@�\
Material		g_material;			// �}�e���A��
Texture			g_texture;			// �e�N�X�`���[
AnimationData	g_anime;			//���`�ۊ�
BlendAnimation		g_blendAnimation;

//==============================================================================
//!	@fn		GameInit
//!	@brief	�Q�[����������
//!	@param	�C���X�^���X�l
//!	@param	�E�C���h�E�n���h���l
//!	@param	�E�C���h�E�T�C�Y���i�␳�ς݂̒l�j
//!	@param	�E�C���h�E�T�C�Y�����i�␳�ς݂̒l�j
//!	@param�@�t���X�N���[���t���O�@true ; �t���X�N���[���@false : �E�C���h�E
//!	@retval	true �����@false ���s
//==============================================================================
bool GameInit(HINSTANCE hinst, HWND hwnd, int width, int height, bool fullscreen)
{
	// DirectX11 �O���t�B�b�N�X������
	bool sts = CDirectXGraphics::GetInstance()->Init(hwnd, width, height, fullscreen);
	if (!sts) {
		MessageBox(nullptr, "CDirectXGraphics Init�G���[", "error!!", MB_OK);
		return false;
	}

	// �A���t�@�u�����f�B���O�I��
	CDirectXGraphics::GetInstance()->TurnOnAlphaBlending();

	// �J����
	XMFLOAT3 eye = { 0.0f,100.0f,-300.0f };
	XMFLOAT3 lookat = { 0.0f,100.0f,0.0f };
	XMFLOAT3 up = { 0.0f,1.0f,0.0f };

	// DIRECTINPUT������ 
	CCamera::GetInstance()->Init(1.0f, 10000.0f, XM_PI / 2.0f, SCREEN_X, SCREEN_Y, eye, lookat, up);

	// �萔�o�b�t�@������
	DX11SetTransform::GetInstance()->Init();

	// ���p�̒萔�o�b�t�@������
	Skeleton::InitConstantBufferBoneMatrix(
		CDirectXGraphics::GetInstance()->GetDXDevice());

	// DIRECTINPUT������
	CDirectInput::GetInstance().Init(hinst, hwnd, width, height);

	// ���C�g������
	XMFLOAT4 lightpos(0, 0, 1, 0);
	g_directionallight.Init(eye, lightpos);

	AssimpScene assimpScene;

	// �f�[�^�ǂݍ���
	assimpScene.Init("assets/ThirdPerson.fbx");

	// ���b�V���f�[�^�ǂݍ���
	g_meshData.Load(&assimpScene);

	// �f�[�^����X�P���g���ǂݍ���
	g_skeleton.Load(&assimpScene);

	g_animation.SetSkeleton(&g_skeleton);

	assimpScene.Exit();

	const char* animNames[3] =
	{
		"assets/ThirdPersonIdle.fbx",
		"assets/ThirdPersonWalk.fbx",
		"assets/ThirdPersonRun.fbx"
	};

	for (int i = 0; i < 3; i++)
	{
		assimpScene.Init(animNames[i]);
		std::unique_ptr<AnimationClip> ptr;
		ptr = std::make_unique<AnimationClip>();
		ptr.get()->Load(&assimpScene, 0);
		g_animationClips.emplace_back(std::move(ptr));
		assimpScene.Exit();
	}

	//// �u�����h�A�j���[�V�����쐬
	g_blendAnimation.AddBlendSample(g_animationClips[0].get(), 0.0f);
	g_blendAnimation.AddBlendSample(g_animationClips[1].get(), 0.5f);
	g_blendAnimation.AddBlendSample(g_animationClips[2].get(), 1.0f);
	g_animation.AddAnimationClips(&g_blendAnimation);

	// �A�j���[�V�����ɒǉ�
	const int animClipNum = g_animationClips.size();
	for (int i = 0; i < animClipNum; i++)
	{
		g_animation.AddAnimationClips(g_animationClips[i].get());
	}



	// �V�F�[�_�ǂݍ���
	g_material.LoadShader("shader/vsoneskin.fx", "shader/graymanps.fx");

	// �e�N�X�`���[�ǂݍ���
	g_texture.Load("GraymanMaskTex.png", "assets");

	// �P�ʍs��ɂ���
	DX11MtxIdentity(g_mtxplayer);

	return	true;
}

//==============================================================================
//!	@fn		GameInput
//!	@brief	�Q�[�����͌��m����
//!	@param�@	
//!	@retval	�Ȃ�
//==============================================================================
void GameInput() {
	// ���͂��󂯕t����
	CDirectInput::GetInstance().GetKeyBuffer();
}

//==============================================================================
//!	@fn		GameUpdate
//!	@brief	�Q�[���X�V����
//!	@param�@	
//!	@retval	�Ȃ�
//==============================================================================
void GameUpdate() {

	static XMFLOAT3 angle = { 90.0f,0.0f,0.0f }; // �p�x���

	static XMFLOAT3 trans = { 0.0f,0.0f,0.0f }; // ���s�ړ���

	static float zpos = 0.0f;

	static int idx = 0;

	static float blendParam = 0.0f;

	if (CDirectInput::GetInstance().CheckKeyBuffer(DIK_RIGHT)) {
		angle.y++;
	}
	if (CDirectInput::GetInstance().CheckKeyBuffer(DIK_LEFT)) {
		angle.y--;
	}

	if (CDirectInput::GetInstance().CheckKeyBuffer(DIK_F1)) {
		zpos += 3.0f;
	}
	if (CDirectInput::GetInstance().CheckKeyBuffer(DIK_F2)) {
		zpos -= 3.0f;
	}

	// �p���X�V
	DX11MakeWorldMatrix(g_mtxplayer, angle, trans);
	g_mtxplayer._43 = zpos;


	// �u�����h�p�p�����[�^�X�V
	if (CDirectInput::GetInstance().CheckKeyBuffer(DIK_UP)) {
		blendParam += 0.01f;
	}
	if (CDirectInput::GetInstance().CheckKeyBuffer(DIK_DOWN)) {
		blendParam -= 0.01f;
	}

	g_animation.SetBlendParameter(blendParam);
	blendParam = g_animation.GetBlendParameter();

	// �A�j���[�V�����؂�ւ�
	if (CDirectInput::GetInstance().CheckKeyBufferTrigger(DIK_SPACE)) {
		int animno = g_animation.GetMotionIndex();
		animno++;
		unsigned int animnummax = g_animation.GetMotionNum();
		if (animno >= animnummax) {
			animno = 0;
		}
		g_animation.Play(animno);
		g_animation.SetBlendParameter(0.0f);
	}

	//g_model.Update(animno,			// �A�j���[�V�����ԍ�
	//	g_mtxplayer);				// ���f���\���ʒu�Ǝp��

	/*DX11MakeWorldMatrix(g_mtxenemy, angle[1], trans[1]);
	g_mtxenemy._43 = zpos;*/

	// �A�j���[�V�����X�s�[�h����
	static float animSpeed = 1.0f;
	if (CDirectInput::GetInstance().CheckKeyBuffer(DIK_Q)) {
		animSpeed -= 0.01f;
	}
	if (CDirectInput::GetInstance().CheckKeyBuffer(DIK_E)) {
		animSpeed += 0.01f;
	}
	g_animation.SetMotionSpeed(animSpeed);

	// ������
	// 60fps�Ɖ��肵�čX�V
	float fps = 30;
	g_animation.UpdateAnimation(1.0f / fps);
}

//==============================================================================
//!	@fn		GameRender
//!	@brief	�Q�[���X�V����
//!	@param�@	
//!	@retval	�Ȃ�
//==============================================================================
void GameRender() {

	// �^�[�Q�b�g�o�b�t�@�N���A	
	float ClearColor[4] = { 0.0f, 0.0f, 1.0f, 1.0f }; //red,green,blue,alpha�i�q�f�a�̊e�l��0.0�`1.0�Ŏw��j

	// �f�o�C�X�R���e�L�X�g�擾
	ID3D11DeviceContext* devcontext;
	devcontext = CDirectXGraphics::GetInstance()->GetImmediateContext();

	// �����_�[�^�[�Q�b�g�r���[�擾
	ID3D11RenderTargetView* rtv;
	rtv = CDirectXGraphics::GetInstance()->GetRenderTargetView();

	// �^�[�Q�b�g�o�b�t�@�N���A
	devcontext->ClearRenderTargetView(
		rtv,			// �����_�[�^�[�Q�b�g�r���[
		ClearColor);	// �N���A����J���[�l

	// �f�v�X�X�e���V���r���[�擾
	ID3D11DepthStencilView* dstv;
	dstv = CDirectXGraphics::GetInstance()->GetDepthStencilView();

	// Z�o�b�t�@�A�X�e���V���o�b�t�@�N���A
	devcontext->ClearDepthStencilView(
		dstv,			// �f�v�X�X�e���V���r���[
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
		1.0f,			// �y�o�b�t�@��1.0�ŃN���A����(0.0f�`1.0f)
		0);				// �X�e���V���o�b�t�@��0�ŃN���A����

	XMFLOAT4X4 mtx;		// �s��

	// �r���[�ϊ��s��Z�b�g
	mtx = CCamera::GetInstance()->GetCameraMatrix();
	DX11SetTransform::GetInstance()->SetTransform(DX11SetTransform::TYPE::VIEW, mtx);

	// �v���W�F�N�V�����ϊ��s��Z�b�g
	mtx = CCamera::GetInstance()->GetProjectionMatrix();
	DX11SetTransform::GetInstance()->SetTransform(DX11SetTransform::TYPE::PROJECTION, mtx);



	// ���f���`��

	g_animation.UpdateConstantBufferBoneMatrix();
	g_material.SetShader();
	g_texture.SetTexture(0);
	DX11SetTransform::GetInstance()->SetTransform(DX11SetTransform::TYPE::WORLD, g_mtxplayer);
	g_meshData.Draw(devcontext);

	// �`��㏈��
	IDXGISwapChain* swapchain;
	swapchain = CDirectXGraphics::GetInstance()->GetSwapChain();
	swapchain->Present(
		0,		// �t���[���̕\���𐂒������̋󔒂Ɠ�����������@���w�肷�鐮���B
		0);		// �X���b�v�`�F�[���\���I�v�V�������܂ސ����l�B 
				// �����̃I�v�V������DXGI_PRESENT�萔�Œ�`����Ă��܂�
}

//==============================================================================
//!	@fn		GameMain
//!	@brief	�Q�[�����[�v����
//!	@param�@	
//!	@retval	�Ȃ�
//==============================================================================
void GameMain()
{
	GameInput();					// ����
	GameUpdate();					// �X�V
	GameRender();					// �`��
}

//==============================================================================
//!	@fn		GameExit
//!	@brief	�Q�[���I������
//!	@param�@	
//!	@retval	�Ȃ�
//==============================================================================
void GameExit()
{
	// ���f���I������
	//g_model.Uninit();

	// �����I������
	g_directionallight.Uninit();

	Skeleton::UninitConstantBufferBoneMatrix();

	// DirectX11 �O���t�B�b�N�X�I������
	CDirectXGraphics::GetInstance()->Exit();
}

//******************************************************************************
//	End of file.
//******************************************************************************



//*****************************************************************************
//ImGuiData
//*****************************************************************************
// IMGUI�E�C���h�E
void imguidebug() {
	//Demo_DragPoints();
	static int listNo;

	if (ImGui::Button(u8"�A�j���[�V����"))
	{
		listNo = animetion;
	}

	if (ImGui::Button(u8"�^�C�����C��"))
	{
		listNo = timeline;
	}

	if (listNo == animetion)
	{
		g_anime.Demo_DragPoints();
	}

	if (listNo == timeline)
	{
		g_timeline.Demo_TimelineGraph();
	}

	//���W��DragGui
	ImGui::Begin(u8"���W");
	static float slider1 = 0.0f;
	static float slider2 = 0.0f;
	slider1 = g_anime.g_monster.GetPos().x;
	slider2 = g_anime.g_monster.GetPos().y;
	static char text1[8] = "";

	ImGui::Text("fps: %.2f", &g_monster.hp);
	ImGui::SliderFloat("slider 1", &slider1, -100, 100);

	FileSelection();

	DirectX::XMFLOAT3 outputpos = { 0,0,0 };

	outputpos.x = slider1;
	outputpos.y = slider2;

	g_monster.SetPos(outputpos);
	ImGui::InputText("textbox 1", text1, sizeof(text1));
	if (ImGui::Button("button 1")) {
		slider1 = 0.0f;
		strcpy(text1, "button 1");
	}

	ImGui::End();
}
