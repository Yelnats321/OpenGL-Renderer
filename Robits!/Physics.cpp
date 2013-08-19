#include "stdafx.h"
#include "Physics.h"

Physics::Physics(){
	broadphase = new btDbvtBroadphase();
	collisionConfiguration = new btDefaultCollisionConfiguration();
	dispatcher = new btCollisionDispatcher(collisionConfiguration);
	solver = new btSequentialImpulseConstraintSolver;
	dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher,broadphase,solver,collisionConfiguration);
	dynamicsWorld->setGravity(btVector3(0,-10,0));

	collisionShapes["ground"] = new btBoxShape(btVector3(100,0,100));

	btDefaultMotionState* groundMotionState = new btDefaultMotionState(btTransform(btQuaternion(0,0,0,1), btVector3(0,0,0)));
	btRigidBody::btRigidBodyConstructionInfo groundRigidBodyCI(0,groundMotionState,collisionShapes["ground"]);
	rigidBodies["ground"] = new btRigidBody(groundRigidBodyCI);
	dynamicsWorld->addRigidBody(rigidBodies["ground"]);


	/*collisionShapes["fall"] = new btSphereShape(1);
	btDefaultMotionState* fallMotionState = new btDefaultMotionState(btTransform(btQuaternion(0,0,0,1),btVector3(0,50,0)));
	btScalar mass = 1;
	btVector3 fallInertia(0,0,0);
	collisionShapes["fall"]->calculateLocalInertia(mass,fallInertia);
	btRigidBody::btRigidBodyConstructionInfo fallRigidBodyCI(mass,fallMotionState,collisionShapes["fall"],fallInertia);
	rigidBodies["fall"] = new btRigidBody(fallRigidBodyCI);
	dynamicsWorld->addRigidBody(rigidBodies["fall"]);*/
}

Physics::~Physics(){
	std::cout <<dynamicsWorld->getNumCollisionObjects();
	for(auto i: rigidBodies){
		dynamicsWorld->removeRigidBody(i.second);
		delete i.second->getMotionState();
		delete i.second;
	}
	for(auto i: collisionShapes)
		delete i.second;
	delete dynamicsWorld;
	delete solver;
	delete dispatcher;
	delete collisionConfiguration;
	delete broadphase;
}

void Physics::addRigidBody(btRigidBody * rb){
	dynamicsWorld->addRigidBody(rb);
}

void Physics::removeRigidBody(btRigidBody * rb){
	dynamicsWorld->removeRigidBody(rb);
}

void Physics::update(btScalar dt){
	dynamicsWorld->stepSimulation(dt, 2);
	
	/*btTransform trans;
	rigidBodies["fall"]->getMotionState()->getWorldTransform(trans);
	std::cout << "sphere height: " << trans.getOrigin().getY() << std::endl;*/
}