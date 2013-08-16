#pragma once

class Player;

class Input{
	GLFWwindow * window;
	Player & player;
public:
	Input(Player &);
	void setWindow(GLFWwindow *);
	void update(float);
};