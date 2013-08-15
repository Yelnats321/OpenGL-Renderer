#pragma once

#define _USE_MATH_DEFINES

#include <array>
#include <glm/glm.hpp>

class Player{
	glm::mat4 camMatrix;
	glm::vec3 camPosition;
	float horizontalAngle, verticalAngle;
	//for camera testing doo doo
	glm::vec3 savedPos;
	glm::mat4 savedMat;
public:
	Player();
	void update(std::array<bool, 7> &, float, float, float);
	const glm::mat4 & getCameraMatrix() const;
	const glm::vec3 & getCamPos() const;
	//For testing purposes
	const glm::mat4 & getSavedMatrix() const;
	const glm::vec3 & getSavedPos() const;
};