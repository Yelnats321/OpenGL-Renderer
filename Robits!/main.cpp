#include "stdafx.h"

#include "Physics.h"
#include "Graphics.h"
#include "Model.h"
#include "Input.h"
#include "Player.h"
#include "Settings.h"

int main(){
	Physics physics;
	Graphics graphics;	Player player(graphics, physics);	graphics.setPlayer(&player);	Input input(player, graphics.getWindow());	/*Model * dragon = graphics.addModel("sponza-x\\sponza.obj");
	dragon->setScale(0.001, 0.001, 0.001);*/
	//graphics.addStaticModel("sponza-x/sponza.obj");
	graphics.addDynamicModel("assets/contraption.obj", physics);
	//floor2->setPosition(0,5,0);
	double lastTime = glfwGetTime();
	double acumulator = 0.0;
	while( graphics.isOpen() ){
		double currentTime = glfwGetTime();
		float deltaTime = float(currentTime - lastTime);
		lastTime = currentTime;
		acumulator += deltaTime;
		int steps = 0;
		while(acumulator >= Settings::Timestep){
			acumulator -= Settings::Timestep;
			input.update(Settings::Timestep);
			physics.update(Settings::Timestep);
			++steps;
		}
		if(steps >=2)
			std::cout << "Took " << steps<< " steps that frame"<<std::endl;
		//updates the input, which in turn updates players instructions
		//add logic update here
		//updates physics
		//draws shit and stuff
		graphics.update();
	}
}