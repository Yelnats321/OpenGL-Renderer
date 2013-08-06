#pragma once

#include "Graphics.h"

class Model{
	glm::mat4 matrix;
	glm::vec3 scayle, position;
	glm::quat rotation;
	std::vector<std::pair<std::string, int> > materials;
	void change();
public:
	Model(GLuint, GLuint, GLuint, int, std::vector<std::pair<std::string, int> > &&);
	GLuint vao, vbo, ebo;
	const int triangles;
	const glm::mat4 & getModelMatrix() const;
	void setScale(glm::vec3 &);
	void setRotation(glm::vec3 &);
	void setPosition(glm::vec3 &);
	/*void scale(glm::vec3 &);
	void rotate(glm::vec3 &);
	void move(glm::vec3 &);*/
};