#include "stdafx.h"

#include "Physics.h"
#include "Graphics.h"
#include "Model.h"
#include "Input.h"
#include "Player.h"
#include "Settings.h"

int main(){
	Physics physics;
	Graphics graphics;
	Player player(graphics, physics);
	graphics.setPlayer(&player);

	/*Model * dragon = graphics.addModel("sponza-x\\sponza.obj");
	dragon->setScale(0.001, 0.001, 0.001);*/
	//graphics.addStaticModel("sponza-x/sponza.obj");
	//graphics.addDynamicModel("assets/contraption.obj", physics);
	auto rigids = graphics.addBus("assets/contraption.obj", physics);
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
			updateInput(player, graphics.getWindow(), rigids);
			physics.update();
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