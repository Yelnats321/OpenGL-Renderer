#pragma once

using namespace physx;

class Physics{
	PxFoundation * pxFoundation;
	PxPhysics * pxPhysics;
	PxScene * pxScene;
	PxDefaultCpuDispatcher * pxCpuDispatcher;
	PxCooking * pxCooking;
	PxControllerManager * pxControllerManager;

	PVD::PvdConnection * pvdConnection;

	/*PxMaterial * floorMat;
	PxRigidStatic * floorRigid;*/
public:
	Physics();
	~Physics();
	void update();
	PxScene * getScene();
	PxPhysics * getPhysics();
	PxControllerManager * getControllerManager();
	PxFoundation * getFoundation();
	void loadRepX(string, PxCollection *, PxCollection *, PxStringTable *);
};