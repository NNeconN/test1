#pragma once
#include "player.h"
#include "CDirectInput.h"

#include <vector>

class AnimationData
{
public:
	AnimationData(); //�R���X�g���N�^

	void Demo_DragPoints(); //���`�ۊǂ���Ă���O���t

	//���`���
	float lerp(float start, float end, float t) {
		return start + t * (end - start);
	}

public:
	std::vector<int> AnimetionKey{}; //�A�j���[�V�����L�[

	Monster g_monster;
private:
	float sy; //leap()�Ōv�Z��̐��l������ϐ� Y���W
	float sx; //leap()�Ōv�Z��̐��l������ϐ� X���W

};