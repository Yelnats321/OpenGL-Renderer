#include "stdafx.h"
#include "Model.h"
//THE ORDER IS TRANSLATE, ROTATE, SCALE
//TRANSLATE TO ORIGIN BEFORE APPLYING ANYTHING

Model::Model(const Mesh * m):mesh(m), scale(1.0){
}

const glm::mat4 & Model::getModelMatrix() const{
	return matrix;
}
//CCW AND IN RADIANS
void Model::setRotation(glm::vec3 & rot){
	rotation = glm::quat(rot);
	change();
}
void Model::setRotation(float x, float y, float z){
	setRotation(glm::vec3(x,y,z));
}

void Model::setScale(glm::vec3 & scal){
	scale = scal;
	change();
}
void Model::setScale(float x, float y, float z){
	setScale(glm::vec3(x,y,z));
}

void Model::setPosition(glm::vec3 & pos){
	position = pos;
	change();
}
void Model::setPosition(float x, float y, float z){
	setPosition(glm::vec3(x,y,z));
}

void Model::change(){
	matrix = glm::mat4(1.0);
	matrix = glm::translate(matrix, position) *glm::toMat4(rotation);
	matrix = glm::scale(matrix, scale);
}

const Mesh * Model::getMesh() const{return mesh;}