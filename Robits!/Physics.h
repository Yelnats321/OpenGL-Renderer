#pragma once

using namespace physx;

class Physics{
	PxFoundation * pxFoundation;
	PxPhysics * pxPhysics;
	PxScene * pxScene;
	PxDefaultCpuDispatcher * pxCpuDispatcher;
	PxCooking * pxCooking;
	PxControllerManager * pxControllerManager;

	/*PxMaterial * floorMat;
	PxRigidStatic * floorRigid;*/
public:
	Physics();
	~Physics();
	void update(float);
	PxScene * getScene();
	PxPhysics * getPhysics();
	PxControllerManager * getControllerManager();
};