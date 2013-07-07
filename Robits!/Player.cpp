#include "Player.h"
#include <iostream>

Player::Player():lpos(0,1,3){
}

const glm::mat4 & Player::getCameraMatrix() const{
	return camera.getMat();
}

const glm::vec3 & Player::getPos() const{
	return lpos;
}

void Player::update(std::array<bool, 7> & keys, float x, float y, float deltaTime){
	camera.update(keys, x, y, deltaTime);
	if(keys[6]){
		lpos = camera.getPos();
	}
}