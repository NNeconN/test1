#pragma once
#include <DirectXMath.h>
#include <vector>

// キーフレーム
struct KeyFrame
{
	DirectX::XMFLOAT4 rotation; // 回転
	DirectX::XMFLOAT3 position; // 座標
	bool shouldSlerpRotation;	// 回転を補間するべきかどうか
	bool shouldLerpPosition;	// 座標を補間するべきかどうか
};