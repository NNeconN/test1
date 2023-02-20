//*****************************************************************************
//!	@file	main.cpp
//!	@brief	
//!	@note	DX11 ひな形
//!	@author	
//*****************************************************************************

//-----------------------------------------------------------------------------
//	Include header files.
//-----------------------------------------------------------------------------
#define NOMINMAX
#include <windows.h>
#include <crtdbg.h>
#include "game.h"

//-----------------------------------------------------------------------------
// マクロの定義
//-----------------------------------------------------------------------------
#define		NAME			"DX11"
#define		TITLE			"DX11 ASSIMPを使ったアニメーション"

//-----------------------------------------------------------------------------
// リンクライブラリの設定
//-----------------------------------------------------------------------------
#pragma comment(lib, "winmm.lib")			// リンク対象ライブラリにwinmm.libを追加
#pragma comment(lib, "d3d11.lib")			// リンク対象ライブラリにd3d11.libを追加
#pragma comment(lib, "dxgi.lib")			// リンク対象ライブラリにdxgi.libを追加
#pragma comment(lib, "directxtex.lib")		// テクスチャを使用するためにdirectxtex.libを追加
#pragma comment(lib, "d3dcompiler.lib")		// シェーダーをコンパイルするためにdirectxtex.libを追加
#pragma comment(lib, "dinput8.lib")			// DIRECTINPUTを使用する為にdinput8.libを追加
#pragma comment(lib, "assimp-vc141-mtd.lib")			// DIRECTINPUTを使用する為にdinput8.libを追加

//-----------------------------------------------------------------------------
// プロトタイプ宣言
//-----------------------------------------------------------------------------
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);			// ウインドウプロシージャ

//-----------------------------------------------------------------------------
// グローバル変数
//-----------------------------------------------------------------------------
int g_nCountFPS = 0;				// ＦＰＳカウンタ

//==============================================================================
//!	@fn		WinMain
//!	@brief	エントリポイント
//!	@param	インスタンスハンドル
//!	@param	意味なし
//!	@param	起動時の引数文字列
//!	@param	ウインドウ表示モード
//!	@retval	TRUE　成功終了/FALSE　失敗終了
//!	@note	
//==============================================================================
int APIENTRY WinMain(HINSTANCE 	hInstance, 		// アプリケーションのハンドル
	HINSTANCE 	hPrevInstance,	// 今のWindowsバージョンでは使われない
	LPSTR 		lpszArgs, 		// 起動時の引数（文字列）
	int 		nWinMode)		// ウインドウ表示モード
{
	HWND			hwnd;						// ウインドウハンドル
	MSG				msg;						// メッセージ構造体
	WNDCLASSEX		wcex;						// ウインドウクラス構造体
	int				width = SCREEN_X;			// ウインドウの幅 計算用ワーク
	int				height = SCREEN_Y;			// ウインドウの高さ 計算用ワーク

	DWORD dwExecLastTime;		// 最後に実行した時間		
	DWORD dwFPSLastTime;		// 最後に計測した時間
	DWORD dwCurrentTime;		// 現在時刻
	DWORD dwFrameCount;			// フレーム数

	// メモリリークを検知
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	// ウインドウクラス情報のセット
	wcex.hInstance = hInstance;			// インスタンス値のセット
	wcex.lpszClassName = NAME;					// クラス名
	wcex.lpfnWndProc = (WNDPROC)WndProc;		// ウインドウメッセージ関数
	wcex.style = 0;					// ウインドウスタイル
	wcex.cbSize = sizeof(WNDCLASSEX);	// 構造体のサイズ
	wcex.hIcon = LoadIcon((HINSTANCE)NULL, IDI_APPLICATION);	// ラージアイコン
	wcex.hIconSm = LoadIcon((HINSTANCE)NULL, IDI_WINLOGO);		// スモールアイコン
	wcex.hCursor = LoadCursor((HINSTANCE)NULL, IDC_ARROW);		// カーソルスタイル
	wcex.lpszMenuName = 0; 					// メニューなし
	wcex.cbClsExtra = 0;					// エキストラなし
	wcex.cbWndExtra = 0;
	wcex.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);			// 背景色白

	if (!RegisterClassEx(&wcex)) return FALSE;	// ウインドウクラスの登録

	RECT	rWindow, rClient;

	hwnd = CreateWindow(
		NAME,							// ウィンドウクラスの名前
		TITLE,							// タイトル
		WS_CAPTION | WS_SYSMENU,		// ウィンドウスタイル
		0, 0,							// ウィンドウ位置 縦, 横(あとで中央に移動させます)
		SCREEN_X, SCREEN_Y,				// ウィンドウサイズ
		HWND_DESKTOP,					// 親ウィンドウなし
		(HMENU)NULL,					// メニューなし
		hInstance,						// インスタンスハンドル
		(LPVOID)NULL);					// 追加引数なし

// ウインドウサイズを再計算（Metricsだけでは、フレームデザインでクライアント領域サイズが変わってしまうので）
	GetWindowRect(hwnd, &rWindow);
	GetClientRect(hwnd, &rClient);
	width = (rWindow.right - rWindow.left) - (rClient.right - rClient.left) + SCREEN_X;
	height = (rWindow.bottom - rWindow.top) - (rClient.bottom - rClient.top) + SCREEN_Y;

	// ウインドウの表示位置をセット
	SetWindowPos(
		hwnd,
		NULL,
		GetSystemMetrics(SM_CXSCREEN) / 2 - width / 2,
		GetSystemMetrics(SM_CYSCREEN) / 2 - height / 2,
		width - 1,
		height - 1,
		SWP_NOZORDER);

	if (!hwnd) return false;

	timeBeginPeriod(1);							// タイマの分解能力を１msにする
	dwExecLastTime = dwFPSLastTime = timeGetTime();
	dwCurrentTime = dwFrameCount = 0;
	// ウインドウを表示する
	ShowWindow(hwnd, nWinMode);
	UpdateWindow(hwnd);

	// ゲームの初期処理
	if (!GameInit(hInstance, hwnd, SCREEN_X, SCREEN_Y, FULLSCREEN)) {
		GameExit();
		MessageBox(hwnd, "ERROR!", "GameInit Error", MB_OK);
		return false;
	}

	// メッセージループ
	while (true)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) != 0)	// メッセージを取得しなかった場合"0"を返す
		{// Windowsの処理
			if (msg.message == WM_QUIT)
			{// PostQuitMessage()が呼ばれて、WM_QUITメッセージが来たらループ終了
				break;
			}
			else
			{
				// メッセージの翻訳と送出
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
		{// DirectXの処理
		 // ＦＰＳ一定化処理

			// 現在時間を取得
			dwCurrentTime = timeGetTime();

			// 0.5秒ごとに実行
			if ((dwCurrentTime - dwFPSLastTime) > 500) {

				// FPSを計算
				g_nCountFPS = (dwFrameCount * 1000) / (dwCurrentTime - dwFPSLastTime);

				// ＦＰＳ計測時間に現座時間をセット
				dwFPSLastTime = dwCurrentTime;

				// フレーム数をゼロクリア
				dwFrameCount = 0;
			}

			// １６ミリ秒経過したら
			if ((dwCurrentTime - dwExecLastTime) >= (1000.0f / 60.0f)) {

				// 最終実行時間に現在時間をセット
				dwExecLastTime = dwCurrentTime;

				// ゲームメイン処理
				GameMain();

				dwFrameCount++;					// フレーム数カウントアップ
			}
		}
	}

	timeEndPeriod(1);							// タイマの分解能力もとに戻す

	GameExit();									// ゲームの終了処理

	return (int)msg.wParam;
}

//==============================================================================
//!	@fn		WindowProc
//!	@brief	ウインドウプロシージャ
//!	@param	ウインドウハンドル
//!	@param	メッセージ
//!	@param	Wパラメータ
//!	@param	Lパラメータ
//!	@retval	終了状況
//==============================================================================
LRESULT WINAPI WndProc(HWND hwnd, 		// ウィンドウハンドル
	UINT message,	// メッセージ識別子
	WPARAM wParam,	// 付帯情報１
	LPARAM lParam)	// 付帯情報２
{

	switch (message) {

	case WM_KEYDOWN:						// キーボード押下	
		switch (wParam) {
		case VK_ESCAPE:
			// ウインドウを終了させる
			DestroyWindow(hwnd);
			break;
		}
		break;
	case WM_DESTROY:						// ウインドウ終了時
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hwnd, message, wParam, lParam);
	}

	return 0;
}

//******************************************************************************
//	End of file.
//******************************************************************************
