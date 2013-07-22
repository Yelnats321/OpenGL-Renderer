#pragma once

#define _USE_MATH_DEFINES

#include <GL/glew.h>
#define GLFW_DLL
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>
#include <string>
#include <array>
using std::vector;
using std::string;

#include "Input.h"
class Model;
class Player;

class Graphics{
	vector<Model *> models;

	GLuint shaderProg;

	glm::mat4 proj;

	GLFWwindow * window;
	Player * player;
	Input input;
public:
	Graphics();
	~Graphics();
	void genShaders(string, string);
	void genBuffers();
	void update(float);
	bool isOpen() const;
	Model * loadModel(string);
	GLFWwindow * getWindow() const;
	void setPlayer(Player *);
};