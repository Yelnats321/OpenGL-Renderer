#include "stdafx.h"

#include "Physics.h"
#include "Graphics.h"
#include "Model.h"
#include "Input.h"
#include "Player.h"

int main(){
	Physics physics;
	Graphics graphics;	Player player(graphics, physics);	graphics.setPlayer(&player);	Input input(player, graphics.getWindow());
	/*Model * dragon = graphics.addModel("sponza-x\\sponza.obj");
	dragon->setScale(0.001, 0.001, 0.001);

	/*Model * monkey = graphics.addModel("monkey.obj");
	monkey->setRotation(3.14/2, 0, 0);
	//monkey->setPosition(2, 0, 0);
	monkey->setScale(0.8, 0.8, 0.8);*/
	Model * floor2 = graphics.addModel("assets/untitled.obj");
	//floor2->setScale(0.3, 0.3, 0.3);
	//floor2->setScale(glm::vec3(0.1));
	/*floor2->setRotation(glm::vec3(3.14/2, 0,0));
	floor2->setPosition(glm::vec3(0,1,1));
	Model * room = graphics.addModel("assets/room.obj");
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
		//updates the input, which in turn updates players instructions
		input.update(deltaTime);
		//add logic update here
		//updates physics
		physics.update(deltaTime);
		//draws shit and stuff
		graphics.update(deltaTime);
	}
}