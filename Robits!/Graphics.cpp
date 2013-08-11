#include "Graphics.h"
#include <iostream>
#include <map>

#include "Settings.h"
#include "Model.h"
#include "Player.h"
#include "Material.h"
#include "AssetLoader.h"

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

	input.setWindow(window);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glEnable(GL_FRAMEBUFFER_SRGB);

	if(!Settings::Wireframe)
		glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	glClearColor( 0.4f, 0.2f, 0.3f, 1.0f );

	setupMainProg("shaders/vert.vert", "shaders/frag.frag");
	setupShadowProg("shaders/passShadow.vert","shaders/passShadow.frag");
	setupQuadProg("shaders/pass.vert", "shaders/pass.frag");
}

Graphics::~Graphics(){
	glDeleteProgram(shadowProgram);
	glDeleteRenderbuffers(1, &renderbuffer);
	glDeleteTextures(1, &shadowTexture);
	glDeleteFramebuffers(1, &framebuffer);

	glDeleteProgram(quadProgram);
	glDeleteBuffers(1, &quadBuffer);
	glDeleteVertexArrays(1, &quadVAO);

	glDeleteTextures(1, &whiteTex);
	glDeleteTextures(1, &blueTex);

	glDeleteProgram(mainProg);
	for(auto i:models){
		glDeleteBuffers(1, &(i->vbo));
		glDeleteBuffers(1, &(i->ebo));
		glDeleteVertexArrays(1, &(i->vao));
		delete i;
	}
	glfwDestroyWindow(window);
    glfwTerminate();
}

void Graphics::setupMainProg(string v, string f){
	mainProg = genShaders(v,f);

	proj = glm::perspective( 50.0f, 800.0f / 600.0f, 0.1f, 100.0f );
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
	glUseProgram(shadowProgram);

	framebuffer = 0;
	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

	glGenTextures(1, &shadowTexture);
	glBindTexture(GL_TEXTURE_2D, shadowTexture);
	glTexImage2D(GL_TEXTURE_2D, 0,GL_DEPTH_COMPONENT16, 800, 600, 0,GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
	glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE,GL_INTENSITY);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowTexture, 0);

	glDrawBuffer(GL_NONE);

	/*glGenRenderbuffers(1, &renderbuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, renderbuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, 800, 600);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, renderbuffer);*/

	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Frame buffer dun goofed";

	glBindFramebuffer(GL_FRAMEBUFFER,0);
}

void Graphics::setupQuadProg(string v, string f){
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
}

void Graphics::update(float deltaTime){
	input.update(deltaTime);

	glViewport(0,0,800,600);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	glUseProgram(shadowProgram);
	/*glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);*/	
	// Compute the MVP matrix from the light's point of view
	glm::mat4 depthProjectionMatrix = /*glm::ortho<float>(-6,6,-6,6,0,20);*/glm::perspective(90.f, 1.f, 0.1f, 50.f);
	//std::cout << player->getPos().x << " " << player->getPos().y << " " << player->getPos().z << std::endl;
	glClear( GL_DEPTH_BUFFER_BIT);
	glBindTexture(GL_TEXTURE_2D, shadowTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
	for(auto i: models){
		glm::mat4 depthMVP = depthProjectionMatrix *player->getSavedMatrix()*i->getModelMatrix();
		glUniformMatrix4fv(glGetUniformLocation(shadowProgram, "depthMVP"),1, GL_FALSE, glm::value_ptr(depthMVP));
		glBindVertexArray(i->vao);
		glDrawElements(GL_TRIANGLES, i->triangles, GL_UNSIGNED_INT,0);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glUseProgram(mainProg);
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	GLint MVPloc = glGetUniformLocation(mainProg, "MVP");
	GLint viewPos = glGetUniformLocation(mainProg, "V");
	GLint modelPos = glGetUniformLocation(mainProg, "M");
	GLint lightPos = glGetUniformLocation(mainProg, "lightPos");
	GLint ambColor = glGetUniformLocation(mainProg, "ambientColor");
	GLint difColor = glGetUniformLocation(mainProg, "diffuseColor");
	GLint specColor = glGetUniformLocation(mainProg, "specularColor");
	GLint shininess = glGetUniformLocation(mainProg, "shineFactor");
	GLint depthMVPos = glGetUniformLocation(mainProg, "depthBiasMVP");
	glUniform3fv(lightPos, 1, glm::value_ptr(player->getSavedPos()));

	const glm::mat4 biasMatrix(
		0.5, 0.0, 0.0, 0.0,
		0.0, 0.5, 0.0, 0.0,
		0.0, 0.0, 0.5, 0.0,
		0.5, 0.5, 0.5, 1.0
		);

	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, shadowTexture);
	for(auto i : models){
		glm::mat4 depthMVP = depthProjectionMatrix *player->getSavedMatrix()*i->getModelMatrix();
		glm::mat4 depthBiasMVP = biasMatrix*depthMVP;
		glUniformMatrix4fv(depthMVPos, 1, GL_FALSE, glm::value_ptr(depthBiasMVP));
		glm::mat4 matrix = proj * player->getCameraMatrix() * i->getModelMatrix();
		/*std::cout << matrix[0][0] << " " << matrix[0][1] << " " << matrix[0][2] << " " <<matrix[0][3] << std::endl;
		std::cout << matrix[1][0] << " " << matrix[1][1] << " " << matrix[1][2] << " " <<matrix[1][3] << std::endl;
		std::cout << matrix[2][0] << " " << matrix[2][1] << " " << matrix[2][2] << " " <<matrix[2][3] << std::endl;
		std::cout << matrix[3][0] << " " << matrix[3][1] << " " << matrix[3][2] << " " <<matrix[3][3] << std::endl;
		std::cout << std::endl;*/
		glUniformMatrix4fv(viewPos, 1, GL_FALSE, glm::value_ptr(player->getCameraMatrix()));
		glUniformMatrix4fv(MVPloc, 1, GL_FALSE, glm::value_ptr(matrix));
		glUniformMatrix4fv(modelPos, 1, GL_FALSE, glm::value_ptr(i->getModelMatrix()));
		glBindVertexArray(i->vao);

		for(auto mats = i->getMatCalls().begin(); mats != i->getMatCalls().end(); ++mats){
			Material * currMat = i->matData(mats->first);
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
			else{
				glBindTexture(GL_TEXTURE_2D, whiteTex);
			}
			glActiveTexture(GL_TEXTURE4);
			if(currMat->map_bump!=0){
				glBindTexture(GL_TEXTURE_2D, currMat->map_bump);
			}
			else
				glBindTexture(GL_TEXTURE_2D, blueTex);

			if(mats+1 == i->getMatCalls().end()){
				glDrawElements(GL_TRIANGLES, i->triangles -mats->second*3, GL_UNSIGNED_INT,(void*)(sizeof(unsigned int) *mats->second*3));
			}
			else{
				glDrawElements(GL_TRIANGLES, (mats+1)->second*3 - mats->second*3, GL_UNSIGNED_INT, (void*)(sizeof(unsigned int) *mats->second*3));
			}
		}
	}
	glViewport(0, 0, 200, 150);

	//glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindVertexArray(quadVAO);
	//glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(quadProgram);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, shadowTexture);
	glBindBuffer(GL_ARRAY_BUFFER, quadBuffer);

	glBindTexture(GL_TEXTURE_2D, shadowTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);
	glDrawArrays(GL_TRIANGLES,0,6);

	glfwPollEvents();
	glfwSwapBuffers(window);
}

Model * Graphics::addModel(string name){
	Model * mod = loadModel(name);
	models.push_back(mod);
	return mod;
}

void Graphics::setPlayer(Player * p){
	player = p;
	input.setPlayer(p);
}

bool Graphics::isOpen() const{
	return !glfwWindowShouldClose(window);
}

GLFWwindow * Graphics::getWindow() const{
	return window;
}