#include "stdafx.h"
#include "Physics.h"

#include "Model.h"

Physics::Physics(){
	static PxDefaultErrorCallback defaultErrorCallback;
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

}

Physics::~Physics(){
	/*pxScene->removeActor(*floorRigid);
	floorRigid->release();
	floorMat->release();*/

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

void Physics::update(float deltaTime){
	pxScene->simulate(deltaTime);
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