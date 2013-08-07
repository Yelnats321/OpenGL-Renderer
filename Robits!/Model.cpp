#include "Model.h"
#include "Material.h"
#include <glm/gtx/quaternion.hpp>

//THE ORDER IS TRANSLATE, ROTATE, SCALE
//TRANSLATE TO ORIGIN BEFORE APPLYING ANYTHING

Model::Model(GLuint a, GLuint b, GLuint e, int tri,vector<std::pair<string, int> > && m, std::map<string, Material *> && mm)
	:vao(a), vbo(b), ebo(e),triangles(tri), scayle(1,1,1), materials(std::move(m)), matMap(std::move(mm)){
}

Model::~Model(){
	//Don't forget to delete all the textures
	for(auto i:matMap){
		glDeleteTextures(1,&i.second->map_Kd);
		delete i.second;
	}
}

const vector<std::pair<string, int>> & Model::getMatCalls() const{
	return materials;
}

Material * Model::matData(string name){
	return matMap[name];
}

const glm::mat4 & Model::getModelMatrix() const{
	return matrix;
}
//CCW AND IN RADIANS
void Model::setRotation(glm::vec3 & rot){
	rotation = glm::quat(rot);
	change();
}

void Model::setScale(glm::vec3 & scal){
	scayle = scal;
	change();
}

void Model::setPosition(glm::vec3 & pos){
	position = pos;
	change();
}

void Model::change(){
	matrix = glm::mat4(1.0);
	matrix = glm::translate(matrix, position) *glm::toMat4(rotation);
	matrix = glm::scale(matrix, scayle);
}