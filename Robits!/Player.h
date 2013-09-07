#pragma once

class Graphics;
class Physics;

using namespace physx;

class Player{
	glm::mat4 camMatrix;
	glm::vec3 camPosition;
	float horizontalAngle, verticalAngle;
	//for camera testing doo doo
	glm::vec3 savedPos;
	Graphics & graphics;
	Physics & physics;

	PxMaterial * physMat;
	//PxRigidBody * physBody;
	PxController * controller;
	PxReal gravity;
public:
	Player(Graphics &, Physics &);
	~Player();
	void update(std::array<bool, 8> &, float, float, float);
	const glm::mat4 & getCameraMatrix() const;
	const glm::vec3 & getCamPos() const;
	//For testing purposes
	const glm::vec3 & getSavedPos() const;
};