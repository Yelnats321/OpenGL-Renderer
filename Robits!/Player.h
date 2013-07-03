#pragma once

#include "Camera.h"
#include "Graphics.h"

class Player{
	Camera camera;
public:
	Player(Graphics &);
	const glm::mat4 & getCameraMatrix() const;
	void update(float, float, float);
};