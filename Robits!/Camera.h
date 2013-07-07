#pragma once

#include "Graphics.h"

class Camera{
	glm::mat4 matrix;
	glm::vec3 position;
	float horizontalAngle, verticalAngle;
public:
	Camera();
	void update(std::array<bool, 7> &, float, float, float);
	const glm::mat4 & getMat() const;
	const glm::vec3 & getPos() const;
};