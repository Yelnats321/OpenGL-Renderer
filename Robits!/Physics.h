#pragma once

class Physics{
	btBroadphaseInterface* broadphase;
	btDefaultCollisionConfiguration* collisionConfiguration;
	btCollisionDispatcher* dispatcher;
	btSequentialImpulseConstraintSolver* solver;
	btDiscreteDynamicsWorld* dynamicsWorld;
	map<string, btCollisionShape *> collisionShapes;
	map<string, btRigidBody *> rigidBodies;
public:
	Physics();
	~Physics();
	void update(btScalar);
	void addRigidBody(btRigidBody *);
	void removeRigidBody(btRigidBody *);
};