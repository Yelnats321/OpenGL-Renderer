#pragma once
class Model;
class Player;

class Graphics{
	vector<Model *> models;

	GLuint mainProg;

	//Texture shit
	GLuint quadBuffer, quadProgram, quadVAO;
	GLuint framebuffer, renderbuffer, shadowProgram, shadowTexture;
	glm::mat4 sideViews[6];

	GLuint whiteTex, blueTex;

	glm::mat4 proj;

	Player * player;

	GLFWwindow * window;
	void setupMainProg(string, string);
	void setupShadowProg(string, string);
	void setupQuadProg(string, string);
public:
	Graphics();
	~Graphics();
	void update();
	bool isOpen() const;
	Model * addModel(string);
	GLFWwindow * getWindow() const;
	void setPlayer(Player *);
	void setLight();
};