#include "stdafx.h"
#include "Physics.h"

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
	pxScene = pxPhysics->createScene(sceneDesc);

	/*floorMat = pxPhysics->createMaterial(0.5, 0.5, 0.1);
	floorRigid = PxCreatePlane(*pxPhysics, PxPlane(PxVec3(0,1,0), 0), *floorMat);
	if(!floorRigid)
		std::cout << "Can't make floor for some reason ";
	floorRigid->setName("floor");
	pxScene->addActor(*floorRigid);*/

	PxDefaultFileInputData data("assets/test1.xml");
	PxCollection* bufferCollection = pxPhysics->createCollection();
	PxCollection* sceneCollection = pxPhysics->createCollection();
	PxStringTable*  stringTable = &PxStringTableExt::createStringTable(pxFoundation->getAllocator()); //stores names?
	/*PxUserReferences* externalRefs = NULL;// pxPhysics->createUserReferences();//we assume there are no external refs
	PxUserReferences * userRefs =  NULL;//  pxPhysics->createUserReferences();//would be used to receive refs and then pass to dependent deserialization calls*/

	repx::deserializeFromRepX(data, *pxPhysics, *pxCooking, stringTable, NULL, *bufferCollection, *sceneCollection, NULL);
	pxPhysics->addCollection(*sceneCollection, *pxScene); //add the scene level objects to the PxScene scene.

	int numActors = sceneCollection->getNbObjects();
	for(int i = 0; i < numActors; ++i){
		PxSerializable * s = sceneCollection->getObject(i);
		if(s->getConcreteType() == PxConcreteType::eRIGID_STATIC){
			PxActor * r = static_cast<PxActor *>(s);
			std::cout << r->getName()<<std::endl;
		}
	}

	bufferCollection->release();
	sceneCollection->release();
	stringTable->release();
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

void Physics::update(float deltaTime){
	pxScene->simulate(deltaTime);
	pxScene->fetchResults(true);
}

PxPhysics * Physics::getPhysics(){return pxPhysics;}
PxScene * Physics::getScene(){return pxScene;}
PxControllerManager * Physics::getControllerManager(){return pxControllerManager;}