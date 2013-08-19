#include "stdafx.h"
#include "Player.h"
#include "Graphics.h"
#include "Settings.h"
#include "Physics.h"


Player::Player(Graphics & g, Physics & p):graphics(g), physics(p), savedPos(0,1,3), camPosition(0.f, 0.3f, 0.f){
	horizontalAngle =0.f;
	verticalAngle = -M_PI/2;
	
	playerShape = new btCapsuleShape(1, 2);
	btDefaultMotionState* playerMotionState = new btDefaultMotionState(btTransform(btQuaternion(0,0,0,1),btVector3(0,1,0)));
	btScalar mass = 60;
	btVector3 fallInertia(0,0,0);
	playerShape->calculateLocalInertia(mass,fallInertia);
	playerBody = new btRigidBody(mass,playerMotionState,playerShape,fallInertia);
	playerBody->setActivationState(DISABLE_DEACTIVATION);
	playerBody->setAngularFactor(btVector3(0,1,0));

	physics.addRigidBody(playerBody);
}

Player::~Player(){
	physics.removeRigidBody(playerBody);
	delete playerBody->getMotionState();
	delete playerBody;
	delete playerShape;
}

const glm::mat4 & Player::getCameraMatrix() const{
	return camMatrix;
}

const glm::vec3 & Player::getSavedPos() const{
	return savedPos;
}const glm::vec3 & Player::getCamPos() const{
	return camPosition;
}

void Player::update(std::array<bool, 8> & keys, float x, float y, float deltaTime){
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
	btTransform trans;
	playerBody->getMotionState()->getWorldTransform(trans);
	camPosition.z = trans.getOrigin().z();
	camPosition.y = trans.getOrigin().y();
	camPosition.x = trans.getOrigin().x();

	btScalar forw = 0, righ = 0;
	// Move forward
	if (keys[0]){
		//camPosition += forward * deltaTime * Settings::Speed;
		forw += forward.z;
		righ += forward.x;
	}
	// Move backward
	if (keys[1]){
		//camPosition -= forward * deltaTime * Settings::Speed;
		forw -= forward.z;
		righ -= forward.x;
	}
	// Strafe right
	if (keys[2]){
		//camPosition += right * deltaTime * Settings::Speed;
		forw += right.z;
		righ += right.x;
	}
	// Strafe left
	if (keys[3]){
		//camPosition -= right * deltaTime * Settings::Speed;
		forw -= right.z;
		righ -= right.x;
	}
	forw *= Settings::Speed;
	righ *= Settings::Speed;
	if(trans.getOrigin().getY() <=2)
		playerBody->applyCentralForce(btVector3(righ, 0, forw));
	//playerBody->setLinearVelocity(btVector3(righ, playerBody->getLinearVelocity().y(), forw));
	/*if(keys[4]){
		camPosition += glm::vec3(0,1,0)*deltaTime *Settings::Speed;
	}
	if(keys[5]){
		camPosition -= glm::vec3(0,1,0)*deltaTime *Settings::Speed;
	}*/
	camMatrix = glm::lookAt(
		camPosition,
		camPosition+direction,
		up
		);

	if(keys[6]){
		savedPos = camPosition;
		graphics.setLight();
	}
	if(keys[7]){
		if(trans.getOrigin().getY() <=2){
			playerBody->applyCentralImpulse(btVector3(0,300,0));
			std::cout << "UH OH SPAGHETTIOS ";
		}
	}


}