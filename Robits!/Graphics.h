#pragma once
#include "OGLWrapper.h"
class Model;
class Player;
class Physics;

class Graphics{
	vector<unique_ptr<Model>> models;

	gl::Program mainProg;

	//Texture shit
	//GLuint quadBuffer, quadProgram, quadVAO;

	gl::Program shadowProgram;
	gl::Framebuffer shadowFramebuffer;
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
	gl::Program colorProg;
	gl::VAO texVAO;
	gl::Buffer texBuffer;
	gl::Framebuffer texFramebuffer;
	gl::Renderbuffer colorDepthBuffer;
	gl::Texture colorTexture, asciiTexture;	

	void setupColorProg(string, string);

	//Voxel stuff
	static const GLuint VoxelSize;
	gl::Program voxelProg, voxelRenderProg;
	gl::VAO voxelRenderVAO;
	gl::Buffer voxelRenderBuffer;
	gl::Texture voxelTexture;
	glm::mat4 voxelLooks[3];
	int voxelCount;
	void setupVoxelProg(string, string, string);
	void genVoxel();
	void renderVoxel();
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