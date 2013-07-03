#include <iostream>

#include "Graphics.h"
#include "Settings.h"
#include "Model.h"
#include "Player.h"

int main(){
	Graphics graphics;	graphics.genShaders();

	Player player(graphics);

	graphics.setPlayer(&player);

	Model * monkey = graphics.loadModel("monkey.obj");
	monkey->setRotation(glm::vec3(3.14/2, 0, 0));
	monkey->setPosition(glm::vec3(1, 0,0));
	monkey->setScale(glm::vec3(0.8, 0.8, 0.8));
	Model * teapot = graphics.loadModel("teapot.obj");
	teapot->setPosition(glm::vec3(0, 0, 2));
	teapot->setScale(glm::vec3(0.01, 0.01, 0.01));
	teapot->setRotation(glm::vec3(3.14/2, 0, 0));
	double lastTime = 0.0;

	while( graphics.isOpen() ){
		double currentTime = glfwGetTime();
		float deltaTime = float(currentTime - lastTime);
		lastTime = currentTime;
		if(deltaTime > 1/60.f)
			deltaTime = 1/60.f;
		graphics.update(deltaTime);
	}
}