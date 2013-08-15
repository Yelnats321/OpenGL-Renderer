#pragma once

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

class Mesh;

class Model{
	glm::mat4 matrix;
	glm::vec3 scale, position;
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
	/*void scale(glm::vec3 &);
	void rotate(glm::vec3 &);
	void move(glm::vec3 &);*/
};