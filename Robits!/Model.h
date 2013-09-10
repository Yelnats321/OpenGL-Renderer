#pragma once

class Mesh;

using namespace physx;

class Model{
protected:
	glm::mat4 matrix;
	glm::vec3 scale, position, origin;
	glm::quat rotation;
	void change();
	const Mesh * mesh;
public:
	Model(const Mesh *);
	const Mesh * getMesh() const;
	const glm::mat4 & getModelMatrix() const;
	void setScale(glm::vec3 &);
	void setScale(float, float, float);
	void setRotation(glm::vec3 &);
	void setRotation(float, float, float);
	void setPosition(glm::vec3 &);
	void setPosition(float, float, float);
	void setTransform(PxTransform &);
	void setOrigin(PxVec3 &);
	/*void scale(glm::vec3 &);
	void rotate(glm::vec3 &);
	void move(glm::vec3 &);*/
};

/*class DynamicModel:public Model{
protected:
	PxRigidActor * actor;
public:
	DynamicModel(const Mesh *, PxRigidActor *);
	~DynamicModel();
};*/