#pragma once
#include "OGLWrapper.h"
class Model;
class Player;
class Physics;

class Graphics{
	vector<unique_ptr<Model>> models;

	GLuint mainProg;

	//Texture shit
	//GLuint quadBuffer, quadProgram, quadVAO;

	GLuint shadowProgram;
	gl::Framebuffer framebuffer;
	gl::Texture shadowTexture;

	glm::mat4 sideViews[6];

	gl::Texture whiteTex, blueTex;

	glm::mat4 proj;

	Player * player;

	GLFWwindow * window;
	void setupMainProg(string, string);
	void setupShadowProg(string, string);
	//void setupQuadProg(string, string);

	//Testing stuff below:
	GLuint colorProg;
	gl::VAO texVAO;
	gl::Buffer texBuffer;
	gl::Framebuffer texFramebuffer;
	gl::Renderbuffer colorDepthBuffer;
	gl::Texture colorTexture, asciiTexture;	

	void setupColorProg(string, string);
public:
	Graphics();
	~Graphics();
	void update();
	bool isOpen() const;
	void addStaticModel(string);
	void addDynamicModel(string, Physics &);
	vector<PxRigidDynamic *> addBus(string, Physics &);
	GLFWwindow * getWindow() const;
	void setPlayer(Player *);
	void setLight();
};