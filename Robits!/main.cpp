#include <iostream>

#include "Graphics.h"
#include "Settings.h"
#include "Model.h"
#include "Player.h"

int main(){
	Graphics graphics;
	Player player;

	graphics.setPlayer(&player);

	//Model * dragon = graphics.addModel("sponza-x\\sponza.obj");
	//dragon->setScale(glm::vec3(0.001, 0.001, 0.001));

	/*Model * monkey = graphics.addModel("monkey.obj");
	monkey->setRotation(glm::vec3(3.14/2, 0, 0));
	//monkey->setPosition(glm::vec3(2, 0, 0));
	monkey->setScale(glm::vec3(0.8, 0.8, 0.8));*/
	Model * floor2 = graphics.addModel("assets/room.obj");
	//floor2->setScale(glm::vec3(0.1));
	//floor2->setRotation(glm::vec3(3.14/2, 0,0));
	//floor2->setPosition(glm::vec3(0,0,1));
	/*Model * floor = graphics.addModel("floor.obj");
	floor->setRotation(glm::vec3(-3.14/2, 0,0));
	floor->setPosition(glm::vec3(0, 0,3));
	floor->setScale(glm::vec3(10, 10, 10));
	//this one is CW
	/*Model * teapot = graphics.addModel("teapot.obj");
	teapot->setPosition(glm::vec3(0, 0, 2));
	teapot->setScale(glm::vec3(0.01, 0.01, 0.01));
	teapot->setRotation(glm::vec3(3.14/2, 0, 0));*/
	double lastTime = 0.0;

	while( graphics.isOpen() ){
		double currentTime = glfwGetTime();
		float deltaTime = float(currentTime - lastTime);
		lastTime = currentTime;
		if(deltaTime > 1/60.f){
			if(deltaTime >1/30.f){
				std::cout << "Slowing down"<<std::endl;
			}
			deltaTime = 1/60.f;
		}
		graphics.update(deltaTime);
	}
}