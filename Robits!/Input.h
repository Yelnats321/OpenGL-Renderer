#pragma once

struct GLFWwindow;
class Player;

class Input{
	GLFWwindow * window;
	Player * player;
public:
	void setPlayer(Player *);
	void setWindow(GLFWwindow *);
	void update(float);
};