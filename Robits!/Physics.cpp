#include "stdafx.h"
#include "Physics.h"
#include "Settings.h"
#include "Model.h"

class errorCallback:public PxErrorCallback{
	virtual void reportError (PxErrorCode::Enum code, const char *message, const char *file, int line){
		std::cout<<"Error error mister robinson!"<<std::endl;
		std::cout<<message<< " in file " << file<< " at line "<<line<<std::endl;
	}
};

Physics::Physics(){
	static errorCallback defaultErrorCallback;
	static PxDefaultAllocator defaultAllocatorCallback;
	pxFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, defaultAllocatorCallback, defaultErrorCallback);
	pxPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *pxFoundation, PxTolerancesScale());
	pxCooking = PxCreateCooking(PX_PHYSICS_VERSION, *pxFoundation, PxCookingParams());
	pxControllerManager = PxCreateControllerManager(*pxFoundation);

	PxSceneDesc sceneDesc(pxPhysics->getTolerancesScale());
	sceneDesc.gravity = PxVec3(0, -9.81f, 0);

	if(!sceneDesc.cpuDispatcher){
		pxCpuDispatcher = PxDefaultCpuDispatcherCreate(1);
		sceneDesc.cpuDispatcher = pxCpuDispatcher;
	}
	if(!sceneDesc.filterShader)
		sceneDesc.filterShader = PxDefaultSimulationFilterShader;
	sceneDesc.flags |= PxSceneFlag::eENABLE_ACTIVETRANSFORMS;
	pxScene = pxPhysics->createScene(sceneDesc);

	/*floorMat = pxPhysics->createMaterial(0.5, 0.5, 0.1);
	floorRigid = PxCreatePlane(*pxPhysics, PxPlane(PxVec3(0,1,0), 0), *floorMat);
	if(!floorRigid)
		std::cout << "Can't make floor for some reason ";
	floorRigid->setName("floor");
	pxScene->addActor(*floorRigid);*/
	pvdConnection = nullptr;
	const char*     pvd_host_ip = "127.0.0.1";  // IP of the PC which is running PVD
	int             port        = 5425;         // TCP port to connect to, where PVD is listening
	unsigned int    timeout     = 100;          // timeout in milliseconds to wait for PVD to respond,
	// consoles and remote PCs need a higher timeout.
	PxVisualDebuggerConnectionFlags connectionFlags = PxVisualDebuggerExt::getAllConnectionFlags();

	// and now try to connect
	pvdConnection = PxVisualDebuggerExt::createConnection(pxPhysics->getPvdConnectionManager(),
		pvd_host_ip, port, timeout, connectionFlags);


}

Physics::~Physics(){
	/*pxScene->removeActor(*floorRigid);
	floorRigid->release();
	floorMat->release();*/
	if(pvdConnection)
		pvdConnection->release();

	pxControllerManager->release();
	pxCooking->release();
	pxCpuDispatcher->release();
	pxScene->release();
	pxPhysics->release();
	pxFoundation->release();
}

void Physics::loadRepX(string name, PxCollection * buffer, PxCollection * scene, PxStringTable * table){
	PxDefaultFileInputData data(name.c_str());

	repx::deserializeFromRepX(data, *pxPhysics, *pxCooking, table, NULL, *buffer, *scene, NULL);
}

void Physics::update(){
	pxScene->simulate(Settings::Timestep);
	pxScene->fetchResults(true);

	PxU32 nbActiveTransforms;
	PxActiveTransform* activeTransforms = pxScene->getActiveTransforms(nbActiveTransforms);
	// update each render object with the new transform
	//std::cout << nbActiveTransforms<<std::endl;
	for (PxU32 i=0; i < nbActiveTransforms; ++i)
	{
		if(activeTransforms[i].userData != NULL){
			Model* model = static_cast<Model*>(activeTransforms[i].userData);
			model->setTransform(activeTransforms[i].actor2World);
		}
	}
}

PxPhysics * Physics::getPhysics(){return pxPhysics;}
PxScene * Physics::getScene(){return pxScene;}
PxControllerManager * Physics::getControllerManager(){return pxControllerManager;}
PxFoundation * Physics::getFoundation(){return pxFoundation;}