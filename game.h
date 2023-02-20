//*****************************************************************************
//!	@file	game.h
//!	@brief	�Q�[������
//!	@note	
//!	@author
//*****************************************************************************
#pragma once

#define		SCREEN_X		800
#define		SCREEN_Y		600
#define		FULLSCREEN      0

//-----------------------------------------------------------------------------
// �v���g�^�C�v�錾
//-----------------------------------------------------------------------------
bool GameInit(HINSTANCE hinst, HWND hwnd, int width, int height, bool fullscreen);
void GameMain();
void GameExit();
void GameInput();
void GameUpdate();
void GameRender();

//******************************************************************************
//	End of file.
//******************************************************************************