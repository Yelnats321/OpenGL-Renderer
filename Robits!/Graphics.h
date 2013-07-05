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
	GLuint vao;

	vector<Model *> models;

	GLuint fragShader, vertShader, shaderProg;

	GLint currTimeLoc, wireframeLoc;
	glm::mat4 proj;
	GLint posAttrib, colAttrib;

	GLFWwindow * window;
	Player * player;
	Input input;
public:
	Graphics();
	~Graphics();
	void genShaders();
	void update(float);
	bool isOpen() const;
	Model * loadModel(string);
	GLFWwindow * getWindow() const;
	void setPlayer(Player *);
};