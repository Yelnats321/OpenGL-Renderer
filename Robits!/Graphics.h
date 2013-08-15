#pragma once

//glew before glfw because gl.h
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <vector>
#include <string>
using std::vector;
using std::string;

#include "Input.h"
#include "Player.h"
class Model;

class Graphics{
	vector<Model *> models;

	GLuint mainProg;

	//Texture shit
	GLuint quadBuffer, quadProgram, quadVAO;
	GLuint framebuffer, renderbuffer, shadowProgram, shadowTexture;
	glm::mat4 sideViews[6];

	GLuint whiteTex, blueTex;

	glm::mat4 proj;

	GLFWwindow * window;
	Player player;
	Input input;
	void setupMainProg(string, string);
	void setupShadowProg(string, string);
	void setupQuadProg(string, string);
public:
	Graphics();
	~Graphics();
	void update(float);
	bool isOpen() const;
	Model * addModel(string);
	GLFWwindow * getWindow() const;
};