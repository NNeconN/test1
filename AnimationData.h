#pragma once
#include "player.h"
#include "CDirectInput.h"

#include <vector>

class AnimationData
{
public:
	AnimationData(); //コンストラクタ

	void Demo_DragPoints(); //線形保管されているグラフ

	//線形補間
	float lerp(float start, float end, float t) {
		return start + t * (end - start);
	}

public:
	std::vector<int> AnimetionKey{}; //アニメーションキー

	Monster g_monster;
private:
	float sy; //leap()で計算後の数値を入れる変数 Y座標
	float sx; //leap()で計算後の数値を入れる変数 X座標

};