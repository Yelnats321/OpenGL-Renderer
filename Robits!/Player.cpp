#include "Player.h"
#include <iostream>

Player::Player(Graphics & g): camera(g){
}

const glm::mat4 & Player::getCameraMatrix() const{
	return camera.getMat();
}

void Player::update(float x, float y, float deltaTime){
	camera.update(x, y, deltaTime);
}