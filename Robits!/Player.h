#pragma once

#include "Camera.h"
#include "Graphics.h"

class Player{
	Camera camera;
	glm::vec3 lpos;
public:
	Player();
	const glm::mat4 & getCameraMatrix() const;
	void update(std::array<bool, 7> &, float, float, float);
	const glm::vec3 & getPos() const;
	const glm::vec3 & getCamPos() const;
};