#include "stdafx.h"
#include "Graphics.h"
#include "Model.h"
#include "Material.h"
#include "AssetLoader.h"
#include "Settings.h"
#include "Player.h"

#include "Physics.h"
#include "ObjFile.h"
#include "Mesh.h"

Graphics::Graphics(){    glfwInit();
	glfwWindowHint(GLFW_SAMPLES, 4); 
	glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 3 );
	glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 3 );
	glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );

	glfwWindowHint( GLFW_RESIZABLE, GL_FALSE );
	window = glfwCreateWindow(800, 600, "OpenGL Window", 0, 0);

	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = GL_TRUE;
	glewInit();


	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glEnable(GL_FRAMEBUFFER_SRGB);

	if(!Settings::Wireframe)
		glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	glClearColor( 0.4f, 0.2f, 0.3f, 1.0f );

	setupMainProg("shaders/vert.vert", "shaders/frag.frag");
	setupShadowProg("shaders/passShadow.vert","shaders/passShadow.frag");
	//setupQuadProg("shaders/pass.vert", "shaders/pass.frag");
}

Graphics::~Graphics(){
	glDeleteProgram(shadowProgram);
	glDeleteRenderbuffers(1, &renderbuffer);
	glDeleteTextures(1, &shadowTexture);
	glDeleteFramebuffers(1, &framebuffer);

	/*glDeleteProgram(quadProgram);
	glDeleteBuffers(1, &quadBuffer);
	glDeleteVertexArrays(1, &quadVAO);*/

	glDeleteTextures(1, &whiteTex);
	glDeleteTextures(1, &blueTex);

	glDeleteProgram(mainProg);

	glfwDestroyWindow(window);
    glfwTerminate();
}

void Graphics::setupMainProg(string v, string f){
	mainProg = genShaders(v,f);

	proj = glm::perspective( 50.0f, 800.0f / 600.0f, 0.01f, 50.0f );
	glUseProgram(mainProg);

	glUniform1f(glGetUniformLocation(mainProg, "ambientIntensity"), Settings::AmbientIntensity);
	
	if(Settings::Wireframe){
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}

	else{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	
	glUniform1i(glGetUniformLocation(mainProg, "ambTex"), 0);
	glUniform1i(glGetUniformLocation(mainProg, "difTex"), 1);
	//glUniform1i(glGetUniformLocation(mainProg, "specTex"), 2);
	glUniform1i(glGetUniformLocation(mainProg, "alphaMask"), 3);
	glUniform1i(glGetUniformLocation(mainProg, "normalMap"), 4);
	glUniform1i(glGetUniformLocation(mainProg, "shadowMap"), 5);

	whiteTex = loadTexture("assets/white.bmp", false);
	blueTex = loadTexture("assets/blue.bmp", false);
}

void Graphics::setupShadowProg(string v, string f){	shadowProgram = genShaders(v, f);

	framebuffer = 0;
	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	glGenTextures(1, &shadowTexture);
	glBindTexture(GL_TEXTURE_CUBE_MAP, shadowTexture);
	for(int i = 0; i < 6; i++){
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i	, 0,GL_DEPTH_COMPONENT16, 800, 800, 0,GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		switch(i){
		case 1://NEGATIVE X
			sideViews[i] = glm::lookAt(glm::vec3(0), glm::vec3(-1,0,0),glm::vec3(0,-1,0));
			break;
		case 3://NEGATIVE Y
			sideViews[i] = glm::lookAt(glm::vec3(0), glm::vec3(0,-1,0),glm::vec3(0,0,-1));
			break;
		case 5://NEGATIVE Z
			sideViews[i] = glm::lookAt(glm::vec3(0), glm::vec3(0,0,-1),glm::vec3(0,-1,0));
			break;
		case 0://POSITIVE X
			sideViews[i] = glm::lookAt(glm::vec3(0), glm::vec3(1,0,0),glm::vec3(0,-1,0));
			break;
		case 2://POSITIVE Y
			sideViews[i] = glm::lookAt(glm::vec3(0), glm::vec3(0,1,0),glm::vec3(0,0,1));
			break;
		case 4://POSITIVE Z
			sideViews[i] = glm::lookAt(glm::vec3(0), glm::vec3(0,0,1),glm::vec3(0,-1,0));
			break;
		}
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	//needed for use of shadow cubemap
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
	glDrawBuffer(GL_NONE);
	/*glGenRenderbuffers(1, &renderbuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, renderbuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, 800, 800);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, renderbuffer);*/
	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Frame buffer dun goofed "<<glCheckFramebufferStatus(GL_FRAMEBUFFER) << std::endl;
}

/*void Graphics::setupQuadProg(string v, string f){
	quadProgram = genShaders(v,f);	glUseProgram(quadProgram);	glUniform1f(glGetUniformLocation(quadProgram, "passTexture"), 0);
	static const GLfloat quad[] = {
		-1.0f, -1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		-1.0f,  1.0f, 0.0f,
		-1.0f,  1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		1.0f,  1.0f, 0.0f,
	};

	glGenVertexArrays(1, &quadVAO);
	glBindVertexArray(quadVAO);

	glGenBuffers(1, &quadBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, quadBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quad), quad, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
}*/

void Graphics::setLight(){
	glViewport(0,0,800,800);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	glUseProgram(shadowProgram);

	GLint depthMVPpos = glGetUniformLocation(shadowProgram, "depthMVP");
	//don't forget to change the function in the program!!!!!!
	//in the fragment shader bro
	//don't go lower than 0.1 near, or else you ahve to finick with the bias 
	glm::mat4 depthProjectionMatrix = glm::perspective(90.f, 1.f, 0.1f, 50.f);
	for(int loop = 0; loop < 6; ++loop){
		glm::mat4 moveMatrix =glm::translate(sideViews[loop], -player->getSavedPos());
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_CUBE_MAP_POSITIVE_X+loop, shadowTexture,0);
		glClear( GL_DEPTH_BUFFER_BIT);
		for(const auto & i: models){
			glm::mat4 depthMVP = depthProjectionMatrix*moveMatrix*i->getModelMatrix();
			glUniformMatrix4fv(depthMVPpos,1, GL_FALSE, glm::value_ptr(depthMVP));
			glBindVertexArray(i->getMesh()->vao);
			glDrawElements(GL_TRIANGLES, i->getMesh()->nbVertices, GL_UNSIGNED_INT,0);
		}
	}
}

void Graphics::update(){
	glUseProgram(mainProg);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glViewport(0,0,800,600);

	GLint MVPloc = glGetUniformLocation(mainProg, "MVP");
	GLint viewPos = glGetUniformLocation(mainProg, "V");
	GLint modelPos = glGetUniformLocation(mainProg, "M");
	GLint lightPos = glGetUniformLocation(mainProg, "lightPos");
	GLint ambColor = glGetUniformLocation(mainProg, "ambientColor");
	GLint difColor = glGetUniformLocation(mainProg, "diffuseColor");
	GLint specColor = glGetUniformLocation(mainProg, "specularColor");
	GLint shininess = glGetUniformLocation(mainProg, "shineFactor");
	GLint toggleTextures = glGetUniformLocation(mainProg, "toggleTextures");
	glUniform3fv(lightPos, 1, glm::value_ptr(player->getSavedPos()));

	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_CUBE_MAP, shadowTexture);
	for(const auto & i : models){
		glm::mat4 matrix = proj * player->getCameraMatrix() * i->getModelMatrix();
		/*std::cout << matrix[0][0] << " " << matrix[0][1] << " " << matrix[0][2] << " " <<matrix[0][3] << std::endl;
		std::cout << matrix[1][0] << " " << matrix[1][1] << " " << matrix[1][2] << " " <<matrix[1][3] << std::endl;
		std::cout << matrix[2][0] << " " << matrix[2][1] << " " << matrix[2][2] << " " <<matrix[2][3] << std::endl;
		std::cout << matrix[3][0] << " " << matrix[3][1] << " " << matrix[3][2] << " " <<matrix[3][3] << std::endl;
		std::cout << std::endl;*/
		auto mesh = i->getMesh();
		glUniformMatrix4fv(viewPos, 1, GL_FALSE, glm::value_ptr(player->getCameraMatrix()));
		glUniformMatrix4fv(MVPloc, 1, GL_FALSE, glm::value_ptr(matrix));
		glUniformMatrix4fv(modelPos, 1, GL_FALSE, glm::value_ptr(i->getModelMatrix()));
		glUniform1f(toggleTextures, mesh->file->useTextures());
		glBindVertexArray(mesh->vao);
		//loop through all the material calls
		for(auto & mats = mesh->matCalls.begin(); mats != mesh->matCalls.end(); ++mats){
			//bind the material data
			const Material * currMat = mesh->file->getMaterial(mats->first);
			glUniform3f(ambColor, currMat->Ka[0], currMat->Ka[1], currMat->Ka[2]);
			glUniform3f(difColor, currMat->Kd[0], currMat->Kd[1], currMat->Kd[2]);
			glUniform3f(specColor, currMat->Ks[0], currMat->Ks[1], currMat->Ks[2]);
			glUniform1f(shininess, currMat->Ns);

			//No ambient texture? use diffuse instead
			glActiveTexture(GL_TEXTURE0);
			if(currMat->map_Ka !=0)
				glBindTexture(GL_TEXTURE_2D, currMat->map_Ka);
			else if(currMat->map_Kd !=0)
				glBindTexture(GL_TEXTURE_2D, currMat->map_Kd);
			else
				glBindTexture(GL_TEXTURE_2D, whiteTex);

			glActiveTexture(GL_TEXTURE1);
			if(currMat->map_Kd !=0)
				glBindTexture(GL_TEXTURE_2D, currMat->map_Kd);
			else
				glBindTexture(GL_TEXTURE_2D, whiteTex);

			//no textures? use white instead
			glActiveTexture(GL_TEXTURE3);
			if(currMat->map_d !=0)
				glBindTexture(GL_TEXTURE_2D, currMat->map_d);
			else
				glBindTexture(GL_TEXTURE_2D, whiteTex);

			//no bump map? use 127,255,127 instead (0,1,0)
			glActiveTexture(GL_TEXTURE4);
			if(currMat->map_bump!=0)
				glBindTexture(GL_TEXTURE_2D, currMat->map_bump);
			else
				glBindTexture(GL_TEXTURE_2D, blueTex);

			//std::cout<< mats->second << " ";
			if(mats+1 != mesh->matCalls.end()){
				glDrawElements(GL_TRIANGLES, (mats+1)->second - mats->second, GL_UNSIGNED_INT, (void*)(sizeof(unsigned int) *mats->second));
				//	std::cout<<(mats+1)->second - mats->second<< " ";
			}
			else{
				glDrawElements(GL_TRIANGLES, mesh->nbVertices -mats->second, GL_UNSIGNED_INT,(void*)(sizeof(unsigned int) *mats->second));
				//	std::cout<<i->getMesh()->getNbIndices()<<std::endl<<std::endl;
			}
		}
	}


	glfwPollEvents();
	glfwSwapBuffers(window);
}
void Graphics::addStaticModel(string name){
	const ObjFile * file = loadFile(name);
	for(const auto & i:file->getMeshes()){
		//Model * mod = new Model(i.second.get());
		models.push_back(unique_ptr<Model>(new Model(i.second.get())));
		models.back().get()->setScale(0.01f,0.01,0.01);
		//std::cout<<i.first<<std::endl;
	}
}

vector<PxRigidDynamic *> Graphics::addBus(string name, Physics & physics){
	const ObjFile * file = loadFile(name);
	map<string, Model*> newmods;
	for(const auto & i:file->getMeshes()){
		//Model * mod = new Model(i.second.get());
		//newmods.emplace(i.first, mod);
		models.push_back(unique_ptr<Model>(new Model(i.second.get())));
		newmods.emplace(i.first, models.back().get());
		//std::cout<<i.first<<std::endl;
	}
	//Model * mod =  new Model(loadMesh(name));
	//models.emplace_back(mods);

	name= name.substr(0, name.find_last_of('.')) + ".xml";

	PxCollection* bufferCollection = physics.getPhysics()->createCollection();
	PxCollection* sceneCollection = physics.getPhysics()->createCollection();
	PxStringTable*  stringTable = &PxStringTableExt::createStringTable(physics.getFoundation()->getAllocator()); //stores names?
	//PxUserReferences* externalRefs = NULL;// pxPhysics->createUserReferences();//we assume there are no external refs
	//PxUserReferences * userRefs =  NULL;//  pxPhysics->createUserReferences();//would be used to receive refs and then pass to dependent deserialization calls

	physics.loadRepX(name, bufferCollection, sceneCollection, stringTable);
	physics.getPhysics()->addCollection(*sceneCollection, *physics.getScene()); //add the scene level objects to the PxScene scene.

	vector<PxRigidDynamic *> rigids;
	int numActors = sceneCollection->getNbObjects();
	for(int i = 0; i < numActors; ++i){
		PxSerializable * s = sceneCollection->getObject(i);
		if(s->getConcreteType() == PxConcreteType::eRIGID_DYNAMIC){
			PxRigidDynamic * r = static_cast<PxRigidDynamic *>(s);
			//std::cout << r->getName()<<std::endl;
			//std::cout<<file->getMeshes().find(r->getName())->second<<std::endl;
			if(newmods.find(r->getName()) != newmods.end()){
				rigids.push_back(r);
				r->userData = newmods[r->getName()];
				newmods[r->getName()]->setOrigin(r->getGlobalPose());
				PxQuat & rotation = r->getGlobalPose().q;
			}
			//r->userData = file->getMeshes().find(r->getName())->second;
			//file->getMeshes().find(r->getName())->second.setOrigin(r->getGlobalPose().p);
		}
	}

	bufferCollection->release();
	sceneCollection->release();
	stringTable->release();
	return rigids;
}

void Graphics::addDynamicModel(string name, Physics & physics){
	const ObjFile * file = loadFile(name);
	map<string, Model*> newmods;
	for(const auto & i:file->getMeshes()){
		//Model * mod = new Model(i.second.get());
		//newmods.emplace(i.first, mod);
		models.push_back(unique_ptr<Model>(new Model(i.second.get())));
		newmods.emplace(i.first, models.back().get());
		//std::cout<<i.first<<std::endl;
	}
	//Model * mod =  new Model(loadMesh(name));
	//models.emplace_back(mods);

	name= name.substr(0, name.find_last_of('.')) + ".xml";

	PxCollection* bufferCollection = physics.getPhysics()->createCollection();
	PxCollection* sceneCollection = physics.getPhysics()->createCollection();
	PxStringTable*  stringTable = &PxStringTableExt::createStringTable(physics.getFoundation()->getAllocator()); //stores names?
	//PxUserReferences* externalRefs = NULL;// pxPhysics->createUserReferences();//we assume there are no external refs
	//PxUserReferences * userRefs =  NULL;//  pxPhysics->createUserReferences();//would be used to receive refs and then pass to dependent deserialization calls

	physics.loadRepX(name, bufferCollection, sceneCollection, stringTable);
	physics.getPhysics()->addCollection(*sceneCollection, *physics.getScene()); //add the scene level objects to the PxScene scene.

	int numActors = sceneCollection->getNbObjects();
	for(int i = 0; i < numActors; ++i){
		PxSerializable * s = sceneCollection->getObject(i);
		if(s->getConcreteType() == PxConcreteType::eRIGID_DYNAMIC){
			PxRigidDynamic * r = static_cast<PxRigidDynamic *>(s);
			//std::cout << r->getName()<<std::endl;
			//std::cout<<file->getMeshes().find(r->getName())->second<<std::endl;
			if(newmods.find(r->getName()) != newmods.end()){
				r->userData = newmods[r->getName()];
				newmods[r->getName()]->setOrigin(r->getGlobalPose());
				PxQuat & rotation = r->getGlobalPose().q;
			}
			//r->userData = file->getMeshes().find(r->getName())->second;
			//file->getMeshes().find(r->getName())->second.setOrigin(r->getGlobalPose().p);
		}
	}

	bufferCollection->release();
	sceneCollection->release();
	stringTable->release();
}

bool Graphics::isOpen() const{
	return !glfwWindowShouldClose(window);
}

GLFWwindow * Graphics::getWindow() const{
	return window;
}

void Graphics::setPlayer(Player * p){
	player = p;
}