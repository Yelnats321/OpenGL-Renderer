#include "Camera.h"
#include "Settings.h"
#include <iostream>
Camera::Camera(Graphics & g) : graphics(g), position(0.f, 0.3f, 0.f){
	horizontalAngle =0.f;
	verticalAngle = -M_PI/2;
}

void Camera::update(float x, float y, float deltaTime){
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
	if (glfwGetKey(graphics.getWindow(), GLFW_KEY_W) == GLFW_PRESS){
		position += forward * deltaTime * Settings::Speed;
	}
	// Move backward
	if (glfwGetKey(graphics.getWindow(),GLFW_KEY_S ) == GLFW_PRESS){
		position -= forward * deltaTime * Settings::Speed;
	}
	// Strafe right
	if (glfwGetKey(graphics.getWindow(),GLFW_KEY_D ) == GLFW_PRESS){
		position += right * deltaTime * Settings::Speed;
	}
	// Strafe left
	if (glfwGetKey(graphics.getWindow(),GLFW_KEY_A ) == GLFW_PRESS){
		position -= right * deltaTime * Settings::Speed;
	}

	if(glfwGetKey(graphics.getWindow(), GLFW_KEY_Q) == GLFW_PRESS){
		position += up*deltaTime *Settings::Speed;
	}
	if(glfwGetKey(graphics.getWindow(), GLFW_KEY_Z) == GLFW_PRESS){
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