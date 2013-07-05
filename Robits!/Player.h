#pragma once

#include "Camera.h"
#include "Graphics.h"

class Player{
	Camera camera;
public:
	const glm::mat4 & getCameraMatrix() const;
	void update(std::array<bool, 6> &, float, float, float);
};