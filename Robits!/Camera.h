#pragma once

#include "Graphics.h"

class Camera{
	glm::mat4 matrix;
	glm::vec3 position;
	float horizontalAngle, verticalAngle;
	Graphics & graphics;
public:
	Camera(Graphics &);
	void update(float, float, float);
	const glm::mat4 & getMat() const;
};