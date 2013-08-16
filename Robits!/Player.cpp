#include "stdafx.h"
#include "Player.h"
#include "Graphics.h"
#include "Settings.h"


Player::Player(Graphics & g):graphics(g), savedPos(0,1,3), camPosition(0.f, 0.3f, 0.f){
	horizontalAngle =0.f;
	verticalAngle = -M_PI/2;
}

const glm::mat4 & Player::getCameraMatrix() const{
	return camMatrix;
}

const glm::vec3 & Player::getSavedPos() const{
	return savedPos;
}const glm::vec3 & Player::getCamPos() const{
	return camPosition;
}

void Player::update(std::array<bool, 7> & keys, float x, float y, float deltaTime){
	horizontalAngle += Settings::MouseSpeed * deltaTime * x;
	verticalAngle   += Settings::MouseSpeed * deltaTime * y;

	if(verticalAngle < - M_PI/2)
		verticalAngle = -M_PI/2;

	else if(verticalAngle > M_PI/2)
		verticalAngle = M_PI/2;

	glm::vec3 direction(
		cos(verticalAngle) * sin(horizontalAngle),
		sin(verticalAngle),
		cos(verticalAngle) * cos(horizontalAngle)
		);
	glm::vec3 right = glm::vec3(
		sin(horizontalAngle - M_PI/2.0f),
		0,
		cos(horizontalAngle - M_PI/2.0f)
		);
	glm::vec3 forward = glm::vec3(
		sin(horizontalAngle),
		0,
		cos(horizontalAngle)
		);

	glm::vec3 up = glm::cross( right, direction );

	// Move forward
	if (keys[0]){
		camPosition += forward * deltaTime * Settings::Speed;
	}
	// Move backward
	if (keys[1]){
		camPosition -= forward * deltaTime * Settings::Speed;
	}
	// Strafe right
	if (keys[2]){
		camPosition += right * deltaTime * Settings::Speed;
	}
	// Strafe left
	if (keys[3]){
		camPosition -= right * deltaTime * Settings::Speed;
	}

	if(keys[4]){
		camPosition += glm::vec3(0,1,0)*deltaTime *Settings::Speed;
	}
	if(keys[5]){
		camPosition -= glm::vec3(0,1,0)*deltaTime *Settings::Speed;
	}

	camMatrix = glm::lookAt(
		camPosition,
		camPosition+direction,
		up
		);

	if(keys[6]){
		savedPos = camPosition;
		graphics.setLight();
	}
}