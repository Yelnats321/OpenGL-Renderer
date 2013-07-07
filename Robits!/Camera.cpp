#include "Camera.h"
#include "Settings.h"
#include <iostream>
Camera::Camera() :position(0.f, 0.3f, 0.f){
	horizontalAngle =0.f;
	verticalAngle = -M_PI/2;
}

void Camera::update(std::array<bool, 7> & keys, float x, float y, float deltaTime){
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
		position += forward * deltaTime * Settings::Speed;
	}
	// Move backward
	if (keys[1]){
		position -= forward * deltaTime * Settings::Speed;
	}
	// Strafe right
	if (keys[2]){
		position += right * deltaTime * Settings::Speed;
	}
	// Strafe left
	if (keys[3]){
		position -= right * deltaTime * Settings::Speed;
	}

	if(keys[4]){
		position += up*deltaTime *Settings::Speed;
	}
	if(keys[5]){
		position -= up*deltaTime *Settings::Speed;
	}

	matrix = glm::lookAt(
		position,
		position+direction,
		up
		);
}

const glm::mat4 & Camera::getMat() const{
	return matrix;
}

const glm::vec3 & Camera::getPos() const{
	return position;
}