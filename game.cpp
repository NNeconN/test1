//*****************************************************************************
//!	@file	game.cpp
//!	@brief	
//!	@note	ゲーム処理
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
// グローバル変数
//-----------------------------------------------------------------------------
//CModel			g_model;			// 主人公モデル
XMFLOAT4X4		g_mtxplayer;		// 主人公の行列

CLight			g_directionallight;	// 平行光源

MeshData		g_meshData;			// メッシュのデータ
Skeleton		g_skeleton;			// モデルのスケルトン
std::vector<std::unique_ptr<AnimationClip>> g_animationClips; // 読み込んだアニメーションクリップの配列
Animation		g_animation;		// アニメーションさせる機能
Material		g_material;			// マテリアル
Texture			g_texture;			// テクスチャー
AnimationData	g_anime;			//線形保管
BlendAnimation		g_blendAnimation;

//==============================================================================
//!	@fn		GameInit
//!	@brief	ゲーム初期処理
//!	@param	インスタンス値
//!	@param	ウインドウハンドル値
//!	@param	ウインドウサイズ幅（補正済みの値）
//!	@param	ウインドウサイズ高さ（補正済みの値）
//!	@param　フルスクリーンフラグ　true ; フルスクリーン　false : ウインドウ
//!	@retval	true 成功　false 失敗
//==============================================================================
bool GameInit(HINSTANCE hinst, HWND hwnd, int width, int height, bool fullscreen)
{
	// DirectX11 グラフィックス初期化
	bool sts = CDirectXGraphics::GetInstance()->Init(hwnd, width, height, fullscreen);
	if (!sts) {
		MessageBox(nullptr, "CDirectXGraphics Initエラー", "error!!", MB_OK);
		return false;
	}

	// アルファブレンディングオン
	CDirectXGraphics::GetInstance()->TurnOnAlphaBlending();

	// カメラ
	XMFLOAT3 eye = { 0.0f,100.0f,-300.0f };
	XMFLOAT3 lookat = { 0.0f,100.0f,0.0f };
	XMFLOAT3 up = { 0.0f,1.0f,0.0f };

	// DIRECTINPUT初期化 
	CCamera::GetInstance()->Init(1.0f, 10000.0f, XM_PI / 2.0f, SCREEN_X, SCREEN_Y, eye, lookat, up);

	// 定数バッファ初期化
	DX11SetTransform::GetInstance()->Init();

	// 骨用の定数バッファ初期化
	Skeleton::InitConstantBufferBoneMatrix(
		CDirectXGraphics::GetInstance()->GetDXDevice());

	// DIRECTINPUT初期化
	CDirectInput::GetInstance().Init(hinst, hwnd, width, height);

	// ライト初期化
	XMFLOAT4 lightpos(0, 0, 1, 0);
	g_directionallight.Init(eye, lightpos);

	AssimpScene assimpScene;

	// データ読み込み
	assimpScene.Init("assets/ThirdPerson.fbx");

	// メッシュデータ読み込み
	g_meshData.Load(&assimpScene);

	// データからスケルトン読み込み
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

	//// ブレンドアニメーション作成
	g_blendAnimation.AddBlendSample(g_animationClips[0].get(), 0.0f);
	g_blendAnimation.AddBlendSample(g_animationClips[1].get(), 0.5f);
	g_blendAnimation.AddBlendSample(g_animationClips[2].get(), 1.0f);
	g_animation.AddAnimationClips(&g_blendAnimation);

	// アニメーションに追加
	const int animClipNum = g_animationClips.size();
	for (int i = 0; i < animClipNum; i++)
	{
		g_animation.AddAnimationClips(g_animationClips[i].get());
	}



	// シェーダ読み込み
	g_material.LoadShader("shader/vsoneskin.fx", "shader/graymanps.fx");

	// テクスチャー読み込み
	g_texture.Load("GraymanMaskTex.png", "assets");

	// 単位行列にする
	DX11MtxIdentity(g_mtxplayer);

	return	true;
}

//==============================================================================
//!	@fn		GameInput
//!	@brief	ゲーム入力検知処理
//!	@param　	
//!	@retval	なし
//==============================================================================
void GameInput() {
	// 入力を受け付ける
	CDirectInput::GetInstance().GetKeyBuffer();
}

//==============================================================================
//!	@fn		GameUpdate
//!	@brief	ゲーム更新処理
//!	@param　	
//!	@retval	なし
//==============================================================================
void GameUpdate() {

	static XMFLOAT3 angle = { 90.0f,0.0f,0.0f }; // 角度情報

	static XMFLOAT3 trans = { 0.0f,0.0f,0.0f }; // 平行移動量

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

	// 姿勢更新
	DX11MakeWorldMatrix(g_mtxplayer, angle, trans);
	g_mtxplayer._43 = zpos;


	// ブレンド用パラメータ更新
	if (CDirectInput::GetInstance().CheckKeyBuffer(DIK_UP)) {
		blendParam += 0.01f;
	}
	if (CDirectInput::GetInstance().CheckKeyBuffer(DIK_DOWN)) {
		blendParam -= 0.01f;
	}

	g_animation.SetBlendParameter(blendParam);
	blendParam = g_animation.GetBlendParameter();

	// アニメーション切り替え
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

	//g_model.Update(animno,			// アニメーション番号
	//	g_mtxplayer);				// モデル表示位置と姿勢

	/*DX11MakeWorldMatrix(g_mtxenemy, angle[1], trans[1]);
	g_mtxenemy._43 = zpos;*/

	// アニメーションスピード制御
	static float animSpeed = 1.0f;
	if (CDirectInput::GetInstance().CheckKeyBuffer(DIK_Q)) {
		animSpeed -= 0.01f;
	}
	if (CDirectInput::GetInstance().CheckKeyBuffer(DIK_E)) {
		animSpeed += 0.01f;
	}
	g_animation.SetMotionSpeed(animSpeed);

	// 仮処理
	// 60fpsと仮定して更新
	float fps = 30;
	g_animation.UpdateAnimation(1.0f / fps);
}

//==============================================================================
//!	@fn		GameRender
//!	@brief	ゲーム更新処理
//!	@param　	
//!	@retval	なし
//==============================================================================
void GameRender() {

	// ターゲットバッファクリア	
	float ClearColor[4] = { 0.0f, 0.0f, 1.0f, 1.0f }; //red,green,blue,alpha（ＲＧＢの各値を0.0～1.0で指定）

	// デバイスコンテキスト取得
	ID3D11DeviceContext* devcontext;
	devcontext = CDirectXGraphics::GetInstance()->GetImmediateContext();

	// レンダーターゲットビュー取得
	ID3D11RenderTargetView* rtv;
	rtv = CDirectXGraphics::GetInstance()->GetRenderTargetView();

	// ターゲットバッファクリア
	devcontext->ClearRenderTargetView(
		rtv,			// レンダーターゲットビュー
		ClearColor);	// クリアするカラー値

	// デプスステンシルビュー取得
	ID3D11DepthStencilView* dstv;
	dstv = CDirectXGraphics::GetInstance()->GetDepthStencilView();

	// Zバッファ、ステンシルバッファクリア
	devcontext->ClearDepthStencilView(
		dstv,			// デプスステンシルビュー
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
		1.0f,			// Ｚバッファを1.0でクリアする(0.0f～1.0f)
		0);				// ステンシルバッファを0でクリアする

	XMFLOAT4X4 mtx;		// 行列

	// ビュー変換行列セット
	mtx = CCamera::GetInstance()->GetCameraMatrix();
	DX11SetTransform::GetInstance()->SetTransform(DX11SetTransform::TYPE::VIEW, mtx);

	// プロジェクション変換行列セット
	mtx = CCamera::GetInstance()->GetProjectionMatrix();
	DX11SetTransform::GetInstance()->SetTransform(DX11SetTransform::TYPE::PROJECTION, mtx);



	// モデル描画

	g_animation.UpdateConstantBufferBoneMatrix();
	g_material.SetShader();
	g_texture.SetTexture(0);
	DX11SetTransform::GetInstance()->SetTransform(DX11SetTransform::TYPE::WORLD, g_mtxplayer);
	g_meshData.Draw(devcontext);

	// 描画後処理
	IDXGISwapChain* swapchain;
	swapchain = CDirectXGraphics::GetInstance()->GetSwapChain();
	swapchain->Present(
		0,		// フレームの表示を垂直方向の空白と同期させる方法を指定する整数。
		0);		// スワップチェーン表示オプションを含む整数値。 
				// これらのオプションはDXGI_PRESENT定数で定義されています
}

//==============================================================================
//!	@fn		GameMain
//!	@brief	ゲームループ処理
//!	@param　	
//!	@retval	なし
//==============================================================================
void GameMain()
{
	GameInput();					// 入力
	GameUpdate();					// 更新
	GameRender();					// 描画
}

//==============================================================================
//!	@fn		GameExit
//!	@brief	ゲーム終了処理
//!	@param　	
//!	@retval	なし
//==============================================================================
void GameExit()
{
	// モデル終了処理
	//g_model.Uninit();

	// 光源終了処理
	g_directionallight.Uninit();

	Skeleton::UninitConstantBufferBoneMatrix();

	// DirectX11 グラフィックス終了処理
	CDirectXGraphics::GetInstance()->Exit();
}

//******************************************************************************
//	End of file.
//******************************************************************************



//*****************************************************************************
//ImGuiData
//*****************************************************************************
// IMGUIウインドウ
void imguidebug() {
	//Demo_DragPoints();
	static int listNo;

	if (ImGui::Button(u8"アニメーション"))
	{
		listNo = animetion;
	}

	if (ImGui::Button(u8"タイムライン"))
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

	//座標のDragGui
	ImGui::Begin(u8"座標");
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
