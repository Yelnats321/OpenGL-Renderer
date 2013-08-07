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

	// Create Vertex Array Object

	proj = glm::perspective( 50.0f, 800.0f / 600.0f, 0.01f, 10000.0f );

	setupMainProg("vert.txt", "frag.txt");
	setupTextureProg("pass.vert","pass.frag");
}

Graphics::~Graphics(){
	glDeleteProgram(texProgram);
	glDeleteBuffers(1, &quadBuffer);
	glDeleteRenderbuffers(1, &depthRenderbuffer);
	glDeleteTextures(1, &renderedTexture);
	glDeleteFramebuffers(1, &framebuffer);
	glDeleteVertexArrays(1, &texVAO);

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

	glUseProgram( mainProg );

	GLint ambInt = glGetUniformLocation(mainProg, "ambientIntensity");
	glUniform1f(ambInt, Settings::AmbientIntensity);
	
	GLint wireframeLoc = glGetUniformLocation(mainProg, "wireframe");
	if(Settings::Wireframe){
		glUniform1i(wireframeLoc, GL_TRUE);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}

	else{
		glUniform1i(wireframeLoc, GL_FALSE);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
}

void Graphics::setupTextureProg(string v, string f){	texProgram = genShaders(v, f);
	glUniform1i( glGetUniformLocation( texProgram, "renderedTexture" ), 0 );

	static const GLfloat quad[] = {
		-1.0f, -1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		-1.0f,  1.0f, 0.0f,
		-1.0f,  1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		1.0f,  1.0f, 0.0f,
	};

	glGenBuffers(1, &quadBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, quadBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quad), quad, GL_STATIC_DRAW);

	framebuffer = 0;
	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

	glGenTextures(1, &renderedTexture);
	glBindTexture(GL_TEXTURE_2D, renderedTexture);
	glTexImage2D(GL_TEXTURE_2D, 0,GL_RGB, 800, 600, 0,GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderedTexture, 0);

	glGenRenderbuffers(1, &depthRenderbuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, depthRenderbuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, 800, 600);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRenderbuffer);

	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Frame buffer dun goofed";

	glGenVertexArrays(1, &texVAO);
	glBindVertexArray(texVAO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glEnableVertexAttribArray(0);
}

void Graphics::update(float deltaTime){
	input.update(deltaTime);

	glViewport(0, 0, 800, 600);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	glUseProgram(mainProg);

	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	GLint MVPloc = glGetUniformLocation(mainProg, "MVP");
	GLint viewPos = glGetUniformLocation(mainProg, "V");
	GLint modelPos = glGetUniformLocation(mainProg, "M");
	GLint lightPos = glGetUniformLocation(mainProg, "lightPos");
	GLint ambColor = glGetUniformLocation(mainProg, "ambientColor");
	GLint difColor = glGetUniformLocation(mainProg, "diffuseColor");
	GLint specColor = glGetUniformLocation(mainProg, "specularColor");
	glUniform3fv(lightPos, 1, glm::value_ptr(player->getPos()));
	glUniform1i(glGetUniformLocation(mainProg, "texSampler"), 0);
	for(auto i : models){
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
			glBindTexture(GL_TEXTURE_2D, currMat->map_Kd);

			if(mats+1 == i->getMatCalls().end()){
				glDrawElements(GL_TRIANGLES, i->triangles -mats->second*3, GL_UNSIGNED_INT,(void*)(sizeof(unsigned int) *mats->second*3));
			}
			else{
				glDrawElements(GL_TRIANGLES, (mats+1)->second*3 - mats->second*3, GL_UNSIGNED_INT, (void*)(sizeof(unsigned int) *mats->second*3));
			}
		}
	}
	//glViewport(0, 0, 400, 300);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindVertexArray(texVAO);
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(texProgram);

	GLint timeLoc = glGetUniformLocation(texProgram, "time");
	glUniform1f(timeLoc, glfwGetTime());

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, renderedTexture);

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