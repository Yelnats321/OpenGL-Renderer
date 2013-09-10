#include "stdafx.h"
#include "Player.h"
#include "Graphics.h"
#include "Settings.h"
#include "Physics.h"
#include "PhysXGLMHelper.h"

Player::Player(Graphics & g, Physics & p):graphics(g), physics(p), savedPos(0,1,3), camPosition(0.f, 0.3f, 0.f){
	horizontalAngle =0.f;
	verticalAngle = -M_PI/2;

	physMat = physics.getPhysics()->createMaterial(0.5, 0.5, 0.1);
	//physBody = PxCreateDynamic(*physics.getPhysics(), PxTransform(PxVec3(0, 2, 0)), PxSphereGeometry(1), *physMat, 1);
	//physics.getScene()->addActor(*physBody);

	PxCapsuleControllerDesc desc;
	desc.height = 1.25;
	desc.radius = 0.25;
	desc.position = PxExtendedVec3(0,5,0);
	desc.material = physMat;
	desc.upDirection = PxVec3(0,1,0);
	//desc.slopeLimit = cosf(M_PI/4);
	desc.density = 10;
	desc.stepOffset = 0.5;
	controller = physics.getControllerManager()->createController(*physics.getPhysics(), physics.getScene(), desc);
	std::cout <<controller->getPosition().x<< " START POS SDASDS "<< controller->getPosition().x - controller->getFootPosition().x - controller->getContactOffset()<<std::endl;
}

Player::~Player(){
	//physics.getScene()->removeActor(*physBody);
	//physBody->release();
	physMat->release();

	controller->release();
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
	PxExtendedVec3 physPos = controller->getPosition();
	//PxVec3 physPos = physBody->getGlobalPose().p;
	camPosition = ptog(physPos);
	camPosition.y += 0.875;

	float forw = 0, righ = 0;
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
	}	/*if(keys[4]){	camPosition += glm::vec3(0,1,0)*deltaTime *Settings::Speed;	}	if(keys[5]){	camPosition -= glm::vec3(0,1,0)*deltaTime *Settings::Speed;	}*/

	PxControllerState state;
	controller->getState(state);
	bool grounded = state.collisionFlags & PxControllerFlag::eCOLLISION_DOWN;
	if(keys[7]){
		/*physics.getScene()->removeActor(*controller->getActor());
		//physBody->addForce(PxVec3(0, 100,0));
		PxSweepHit hitInfo;
		PxVec3 centerPos;
		centerPos.x = controller->getPosition().x;
		centerPos.y = controller->getPosition().y;
		centerPos.z = controller->getPosition().z;
		/*PxVec3 botPos;
		botPos.x = controller->getFootPosition().x;
		botPos.y = controller->getFootPosition().y-controller->getContactOffset();
		botPos.z = controller->getFootPosition().z;
		PxVec3 topPos(centerPos);
		topPos.y = topPos.y + (centerPos.y-botPos.y);*/
		/*bool blockingHit;
		const PxU32 bufferSize = 256;
		PxSweepHit hitBuffer[bufferSize];
		PxI32 nbHits = physics.getScene()->sweepMultiple(PxCapsuleGeometry(0.25+controller->getContactOffset(), 1.25/2), PxTransform(centerPos), PxVec3(0,-1,0), 0.1, PxSceneQueryFlags(PxSceneQueryFlag::eINITIAL_OVERLAP|PxSceneQueryFlag::eINITIAL_OVERLAP_KEEP),
			hitBuffer, bufferSize, blockingHit);
		std::cout << nbHits << " " <<blockingHit<< " ASDSADASDAS DSAD "<<std::endl;*/
		//bool gotHit = physics.getScene()->sweepSingle(PxCapsuleGeometry(0.25+controller->getContactOffset(), 1.25/2), PxTransform(centerPos),-controller->getUpDirection(), 0.1, PxSceneQueryFlags(PxSceneQueryFlag::eINITIAL_OVERLAP|PxSceneQueryFlag::eINITIAL_OVERLAP_KEEP|PxSceneQueryFlag::eNORMAL), hitInfo);
		//bool gotHit = physics.getScene()->sweepSingle(PxCapsuleGeometry(0.25, 1.25), PxTransform(centerPos),PxVec3(0,-1,0), controller->getContactOffset(), PxSceneQueryFlags(), hitInfo);
		if(grounded){
			gravity = 20*Settings::Timestep;
			std::cout << "Ray charles came back true "<<std::endl;
		}
		//physics.getScene()->addActor(*controller->getActor());
	}
	forw *= Settings::Speed * Settings::Timestep;
	righ *= Settings::Speed * Settings::Timestep;
	if(!grounded){
		gravity += physics.getScene()->getGravity().y*Settings::Timestep/10;
		if(gravity < -20*Settings::Timestep)
			gravity = -20 * Settings::Timestep;
	}
	else if(gravity <0)
		gravity = 0;
	//physBody->addForce(PxVec3(righ, 0, forw));
	//POSSIBLY REMOVE DELTA TIME IN FAVOR OF THE TIEMSTEP THING????????
	controller->move(PxVec3(righ, gravity, forw), Settings::MinMoveDist, deltaTime, NULL);


	if(keys[6]){
		savedPos = camPosition;
		graphics.setLight();
		std::cout<<camPosition.x<< " " << camPosition.y<< " " <<camPosition.z<<std::endl;
	}

	camMatrix = glm::lookAt(
		camPosition,
		camPosition+direction,
		up
		);
}