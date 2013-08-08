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
using std::vector;
using std::string;

#include "Input.h"
class Model;
class Player;

class Graphics{
	vector<Model *> models;

	GLuint mainProg;

	//Texture shit
	GLuint quadBuffer, framebuffer, renderbuffer, renderedTexture, texVAO, texProgram;

	GLuint whiteTex;

	glm::mat4 proj;

	GLFWwindow * window;
	Player * player;
	Input input;
	void setupMainProg(string, string);
	void setupTextureProg(string, string);
public:
	Graphics();
	~Graphics();
	void update(float);
	bool isOpen() const;
	Model * addModel(string);
	GLFWwindow * getWindow() const;
	void setPlayer(Player *);
};