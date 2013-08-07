#pragma once

#include "Graphics.h"
#include <map>
struct Material;

class Model{
	glm::mat4 matrix;
	glm::vec3 scayle, position;
	glm::quat rotation;
	//This keeps track of when usemtl is used, and what mtl is to be used
	vector<std::pair<string, int> > materials;
	//This actually stores the material data
	std::map<string, Material *> matMap;
	void change();
public:
	Model(GLuint, GLuint, GLuint, int, vector<std::pair<string, int> > &&, std::map<string, Material *> &&);
	~Model();
	GLuint vao, vbo, ebo;
	const int triangles;
	const glm::mat4 & getModelMatrix() const;
	const vector<std::pair<string, int>> & getMatCalls() const;
	Material * matData(string);
	void setScale(glm::vec3 &);
	void setRotation(glm::vec3 &);
	void setPosition(glm::vec3 &);
	/*void scale(glm::vec3 &);
	void rotate(glm::vec3 &);
	void move(glm::vec3 &);*/
};