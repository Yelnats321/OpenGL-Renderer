#include "Input.h"
#include "Player.h"
#include <GLFW/glfw3.h>

#include <iostream>

Input::Input(Player & p): player(p){
}

void Input::setWindow(GLFWwindow * w){
	window = w;
}

void Input::update(float deltaTime){
	if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, 1);

	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);
	glfwSetCursorPos(window, 800.0/2, 600.0/2);

	std::array<bool, 7> keys= {};
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS){
		keys[0] = true;
	}
	// Move backward
	if (glfwGetKey(window,GLFW_KEY_S ) == GLFW_PRESS){
		keys[1] = true;
	}
	// Strafe right
	if (glfwGetKey(window,GLFW_KEY_D ) == GLFW_PRESS){
		keys[2] = true;
	}
	// Strafe left
	if (glfwGetKey(window,GLFW_KEY_A ) == GLFW_PRESS){
		keys[3] = true;
	}

	if(glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS){
		keys[4] = true;
	}
	if(glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS){
		keys[5] = true;
	}
	if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS){
		keys[6] = true;
	}

	player.update(keys, float(800/2 - xpos), float(600/2 - ypos), deltaTime);
}