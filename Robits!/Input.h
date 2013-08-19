#pragma once

class Player;

class Input{
	GLFWwindow * window;
	Player & player;
public:
	Input(Player &, GLFWwindow *);
	void update(float);
};