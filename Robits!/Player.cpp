#include "Player.h"
#include <iostream>

const glm::mat4 & Player::getCameraMatrix() const{
	return camera.getMat();
}

void Player::update(std::array<bool, 6> & keys, float x, float y, float deltaTime){
	camera.update(keys, x, y, deltaTime);
}