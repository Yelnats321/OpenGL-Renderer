#include "stdafx.h"
#include "Graphics.h"
#include "Model.h"
#include "Material.h"
#include "OGLWrapper.h"
#include "Settings.h"
#include "Player.h"

#include "Physics.h"
#include "ObjFile.h"
#include "Mesh.h"

Graphics::Graphics(){	glfwInit();
	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	window = glfwCreateWindow(800, 600, "OpenGL Window", 0, 0);

	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = GL_TRUE;
	glewInit();


	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glEnable(GL_FRAMEBUFFER_SRGB);
	glClearColor(0.4f, 0.2f, 0.3f, 1.0f);

	if(!Settings::Wireframe)
		glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);

	setupMainProg("shaders/vert.vert", "shaders/frag.frag");
	setupShadowProg("shaders/passShadow.vert", "shaders/passShadow.frag");
	//setupQuadProg("shaders/pass.vert", "shaders/pass.frag");
	setupColorProg("shaders/pass.vert", "shaders/pass.frag");
	setupVoxelProg("shaders/voxel.vert", "shaders/voxel.frag", "shaders/voxel.geo");
}

Graphics::~Graphics(){
	glfwDestroyWindow(window);
	glfwTerminate();
}

void Graphics::setupMainProg(string v, string f){
	mainProg.gen(v, f);

	proj = glm::perspective(50.0f, 800.0f / 600.0f, 0.01f, 50.0f);
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

	whiteTex.gen("assets/white.bmp");
	blueTex.gen("assets/blue.bmp");
}

void Graphics::setupShadowProg(string v, string f){	shadowProgram.gen(v, f);

	shadowFramebuffer.gen();

	glBindFramebuffer(GL_FRAMEBUFFER, shadowFramebuffer);
	shadowTexture.gen();
	glBindTexture(GL_TEXTURE_CUBE_MAP, shadowTexture);
	for(int i = 0; i < 6; i++){
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT16, 800, 800, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		switch(i){
		case 1://NEGATIVE X
			sideViews[i] = glm::lookAt(glm::vec3(0), glm::vec3(-1, 0, 0), glm::vec3(0, -1, 0));
			break;
		case 3://NEGATIVE Y
			sideViews[i] = glm::lookAt(glm::vec3(0), glm::vec3(0, -1, 0), glm::vec3(0, 0, -1));
			break;
		case 5://NEGATIVE Z
			sideViews[i] = glm::lookAt(glm::vec3(0), glm::vec3(0, 0, -1), glm::vec3(0, -1, 0));
			break;
		case 0://POSITIVE X
			sideViews[i] = glm::lookAt(glm::vec3(0), glm::vec3(1, 0, 0), glm::vec3(0, -1, 0));
			break;
		case 2://POSITIVE Y
			sideViews[i] = glm::lookAt(glm::vec3(0), glm::vec3(0, 1, 0), glm::vec3(0, 0, 1));
			break;
		case 4://POSITIVE Z
			sideViews[i] = glm::lookAt(glm::vec3(0), glm::vec3(0, 0, 1), glm::vec3(0, -1, 0));
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
}

void Graphics::setupColorProg(string v, string f){
	colorProg.gen(v, f);
	glUseProgram(colorProg);
	asciiTexture.gen("assets/charMap.png", false, false);
	glUniform1i(glGetUniformLocation(colorProg, "asciiTexture"), 1);

	texFramebuffer.gen();
	glBindFramebuffer(GL_FRAMEBUFFER, texFramebuffer);
	colorTexture.gen();
	glBindTexture(GL_TEXTURE_2D, colorTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 800, 600, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, colorTexture, 0);

	colorDepthBuffer.gen();
	glBindRenderbuffer(GL_RENDERBUFFER, colorDepthBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, 800, 600);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, colorDepthBuffer);
	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Frame buffer dun goofed " << glCheckFramebufferStatus(GL_FRAMEBUFFER) << std::endl;

	glUniform1f(glGetUniformLocation(colorProg, "passTexture"), 0);
	static const GLfloat quad[] = {
		-1.0f, 1.0f, 0.0f,
		-1.0f, -1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,

		-1.0f, 1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		1.0f, 1.0f, 0.0f,
	};

	texVAO.gen();
	glBindVertexArray(texVAO);

	texBuffer.gen();
	glBindBuffer(GL_ARRAY_BUFFER, texBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quad), quad, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

}

void Graphics::setupVoxelProg(string v, string f, string g){
	voxelRenderProg.gen("shaders/voxelRender.vert", "shaders/voxelRender.frag", "shaders/voxelRender.geo");
	voxelProg.gen(v, f, g);

	voxelTexture.gen();
	glBindTexture(GL_TEXTURE_3D, voxelTexture);
	unsigned char * clearData = new unsigned char[VoxelSize* VoxelSize* VoxelSize];
	memset(clearData, 5, VoxelSize* VoxelSize* VoxelSize);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexImage3D(GL_TEXTURE_3D, 0, GL_R8UI, VoxelSize, VoxelSize, VoxelSize, 0, GL_RED_INTEGER, GL_UNSIGNED_BYTE, clearData);
	delete[] clearData;
	clearData = 0;
	/*voxelFramebuffer.gen();
	glBindFramebuffer(GL_FRAMEBUFFER, voxelFramebuffer);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, voxelTexture, 0);	GLuint buffer = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if(buffer != GL_FRAMEBUFFER_COMPLETE){
	std::cout <<"Buffer status: "<< buffer << std::endl;
	}*/
	/*static const GLfloat quadz[] = {
	-1.0f, 1.0f, 0.0f,
	-1.0f, -1.0f, 0.0f,
	1.0f, -1.0f, 0.0f,

	-1.0f, 1.0f, 0.0f,
	1.0f, -1.0f, 0.0f,
	1.0f, 1.0f, 0.0f,
	};

	voxelRenderVAO.gen();
	glBindVertexArray(voxelRenderVAO);

	voxelRenderBuffer.gen();
	std::cout << voxelRenderBuffer;
	glBindBuffer(GL_ARRAY_BUFFER, voxelRenderBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadz), quadz, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	glProgramUniform1f(voxelRenderProg, glGetUniformLocation(voxelRenderProg, "dataTex"), 0);*/
	//auto ortho = glm::ortho<float>(-1920.95f, 1799.91f, -126.442, 1429.43, -1182.81f, 1105.43f);

	//auto ortho = glm::ortho<float>(-8.5215, 13.077, 0, 20.9551, -6.51986, 7.07799);
	//auto ortho = glm::ortho<float>(-10, 15, -5, 25, -10, 10);
	/*auto ortho = glm::mat4{1 / (13.077 + 8.5215), 0, 0, 0,
		0, 1 / (20.9511), 0, 0,
		0, 0, 1 / (7.07799 + 6.51986), 0,
		8.5125 / (13.077 + 8.5215), 0, 6.51986 / (7.07799 + 6.51986), 1};*/
	//	glm::scale(glm::mat4(), glm::vec3(1 / (13.077 + 8.5215), 1 / 20.9511, 1 / (7.07799 + 6.51986)));
	//ortho = glm::translate(ortho, glm::vec3(8.5215, 0, 6.51986));


	glProgramUniform1i(voxelProg, glGetUniformLocation(voxelProg, "voxelSize"), VoxelSize);
}



void Graphics::genVoxel(){
	static bool done = false;
	if(!done){
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, VoxelSize, VoxelSize);
		glUseProgram(voxelProg);
		glBindImageTexture(0, voxelTexture, 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_R8UI);
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);
		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

		//TODO: change this to use all models but single pass
		for(const auto & i : models){
			auto & min = i->getMesh()->minBounds;
			auto & max = i->getMesh()->maxBounds;
			auto & maxed = max - min;

			glm::mat4 move = glm::translate(glm::mat4(1.0), -min - std::max(maxed.x, std::max(maxed.y, maxed.z)) / 2.f);
			//use a bias so that things that are very close to the -1 or 1 are not exactly that
			//might not be needed for conservative rasterization
			const float bias = 0.001;
			glm::mat4 scale = glm::scale(glm::mat4(1.0), glm::vec3((2.f - bias) / std::max(maxed.x, std::max(maxed.y, maxed.z))));
			voxelLooks[0] = glm::mat4(0.f, 0.f, 1.f, 0.f,
									  0.f, 1.f, 0.f, 0.f,
									  1.f, 0.f, 0.f, 0.f, 
									  0, 0, 0, 1) *scale* move;
			voxelLooks[1] = glm::mat4(1.f, 0.f, 0.f, 0.f, 
									  0.f, 0.f, 1.f, 0.f, 
									  0.f, 1.f, 0.f, 0.f,
									  0, 0, 0, 1) * scale* move;
			voxelLooks[2] = scale* move;
			/*std::cout << glm::to_string((min)) << std::endl
				<< glm::to_string(max) << std::endl
				<< glm::to_string(maxed) << std::endl;*/
			glProgramUniformMatrix4fv(voxelProg, glGetUniformLocation(voxelProg, "lookX"), 1, GL_FALSE, glm::value_ptr(voxelLooks[0]));
			glProgramUniformMatrix4fv(voxelProg, glGetUniformLocation(voxelProg, "lookY"), 1, GL_FALSE, glm::value_ptr(voxelLooks[1]));
			glProgramUniformMatrix4fv(voxelProg, glGetUniformLocation(voxelProg, "lookZ"), 1, GL_FALSE, glm::value_ptr(voxelLooks[2]));

			//glm::mat4 matrix = glm::lookAt(glm::vec3{0, 0, 1}, glm::vec3{0, 0, 0}, glm::vec3{0, 1, 0})* glm::scale(glm::mat4(), glm::vec3(1799.91f + 1920.95f / 2, 1429.43 + 126.442 / 2, 1105.43f + 1182.81f / 2));
			glBindVertexArray(i->getMesh()->vao);
			glDrawElements(GL_TRIANGLES, i->getMesh()->nbVertices, GL_UNSIGNED_INT, 0);
		}
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
		done = true;

		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

		unsigned char * data= new unsigned char[VoxelSize * VoxelSize * VoxelSize];

		glPixelStorei(GL_PACK_ALIGNMENT, 1);
		glGetTexImage(GL_TEXTURE_3D, 0, GL_RED_INTEGER, GL_UNSIGNED_BYTE, data);

		voxelCount = 0;
		for(int z = 0; z < VoxelSize; z++){
			for(int y = 0; y < VoxelSize; y++){
				for(int x = 0; x < VoxelSize; x++){
					if(data[x + y * VoxelSize + z*VoxelSize*VoxelSize] == 30){
						voxelCount++;
					}
				}
			}
		}
		std::cout << "Voxels: " << voxelCount << std::endl;
		auto voxelArr = new glm::vec3[voxelCount];
		int vox = 0;
		bool first = true;
		for(int z = 0; z < VoxelSize; z++){
			for(int y = 0; y < VoxelSize; y++){
				for(int x = 0; x < VoxelSize; x++){
					if(data[x + y * VoxelSize + z*VoxelSize*VoxelSize] == 30){
						//voxelArr[vox] = {x, y*(1429.43 + 126.442) / (1799.91f + 1920.95f), z* (1105.43f + 1182.81f) / (1799.91f + 1920.95f)};
						voxelArr[vox] = {x, y, z};
						if(first)
						std::cout << x<< " " << y<< " "<<z<< std::endl;
						first = false;
						voxelArr[vox] += 0.5;
						vox++;
					}
					else if(data[x + y * VoxelSize + z*VoxelSize*VoxelSize] != 5){
						std::cout << x << " " << y << " " << z << " " << (int)data[x + y * VoxelSize + z*VoxelSize*VoxelSize];
						throw std::exception("Bad write to texture ");
					}
				}
			}
		}
		if(voxelCount)
			std::cout << glm::to_string(voxelArr[0]) << std::endl;
		assert(vox == voxelCount);
		voxelRenderVAO.gen();
		glBindVertexArray(voxelRenderVAO);

		voxelRenderBuffer.gen();
		glBindBuffer(GL_ARRAY_BUFFER, voxelRenderBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*voxelCount, voxelArr, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(0);
		delete[] data;
		delete[] voxelArr;
	}
}

void Graphics::renderVoxel(){
	glViewport(0, 0, 800, 600);
	glUseProgram(voxelRenderProg);

	//TODO: change this to use all models
	const auto &i = models.front();
	//ortho = generateOrthoVoxel(i->getMesh()->minBounds, i->getMesh()->maxBounds);
	auto maxed = i->getMesh()->maxBounds - i->getMesh()->minBounds;
	//ortho = glm::scale(i->getModelMatrix(), glm::vec3(1.f/VoxelSize));
	float maxAxis = std::max(maxed.x, std::max(maxed.y, maxed.z));
	//std::cout << glm::to_string(maxed) << std::endl;
	auto ortho = glm::translate(i->getModelMatrix(), i->getMesh()->minBounds);

	ortho = glm::scale(ortho, glm::vec3(maxAxis));

	ortho = glm::scale(ortho, glm::vec3(1.f/ (float)VoxelSize));
	//ortho = i->getModelMatrix() * ortho;

	//ortho = glm::translate(ortho, i->getMesh()->minBounds);
	//ortho = glm::scale(ortho, glm::vec3(2.f/VoxelSize));

	//std::cout << glm::to_string(ortho*glm::vec4(0, 0, 0, 1));
	//ortho = glm::scale(glm::mat4(), glm::vec3(2.f / VoxelSize));
	glProgramUniformMatrix4fv(voxelRenderProg, glGetUniformLocation(voxelRenderProg, "VP"), 1, GL_FALSE, glm::value_ptr(proj*player->getCameraMatrix()*ortho));

	glBindVertexArray(voxelRenderVAO);

	glDrawArrays(GL_POINTS, 0, voxelCount);
}

void Graphics::setLight(){
	glViewport(0, 0, 800, 800);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFramebuffer);
	glUseProgram(shadowProgram);
	glEnable(GL_DEPTH_TEST);
	GLint depthMVPpos = glGetUniformLocation(shadowProgram, "depthMVP");
	//don't forget to change the function in the program!!!!!!
	//in the fragment shader bro
	//don't go lower than 0.1 near, or else you ahve to finick with the bias 
	glm::mat4 depthProjectionMatrix = glm::perspective(90.f, 1.f, 0.1f, 100.f);
	for(int loop = 0; loop < 6; ++loop){
		glm::mat4 moveMatrix = glm::translate(sideViews[loop], -player->getSavedPos());
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_CUBE_MAP_POSITIVE_X + loop, shadowTexture, 0);
		glClear(GL_DEPTH_BUFFER_BIT);
		for(const auto & i : models){
			glm::mat4 depthMVP = depthProjectionMatrix*moveMatrix*i->getModelMatrix();
			glUniformMatrix4fv(depthMVPpos, 1, GL_FALSE, glm::value_ptr(depthMVP));
			glBindVertexArray(i->getMesh()->vao);
			glDrawElements(GL_TRIANGLES, i->getMesh()->nbVertices, GL_UNSIGNED_INT, 0);
		}
	}
}

void Graphics::update(){
	genVoxel();

	glBindFramebuffer(GL_FRAMEBUFFER, texFramebuffer);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glUseProgram(mainProg);
	glViewport(0, 0, 800, 600);

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
		for(auto mats = mesh->matCalls.begin(); mats != mesh->matCalls.end(); ++mats){
			//bind the material data
			const Material * currMat = mesh->file->getMaterial(mats->first);
			glUniform3f(ambColor, currMat->Ka[0], currMat->Ka[1], currMat->Ka[2]);
			glUniform3f(difColor, currMat->Kd[0], currMat->Kd[1], currMat->Kd[2]);
			glUniform3f(specColor, currMat->Ks[0], currMat->Ks[1], currMat->Ks[2]);
			glUniform1f(shininess, currMat->Ns);

			//No ambient texture? use diffuse instead
			glActiveTexture(GL_TEXTURE0);
			if(currMat->map_Ka)
				glBindTexture(GL_TEXTURE_2D, currMat->map_Ka);
			else if(currMat->map_Kd)
				glBindTexture(GL_TEXTURE_2D, currMat->map_Kd);
			else
				glBindTexture(GL_TEXTURE_2D, whiteTex);

			glActiveTexture(GL_TEXTURE1);
			if(currMat->map_Kd)
				glBindTexture(GL_TEXTURE_2D, currMat->map_Kd);
			else
				glBindTexture(GL_TEXTURE_2D, whiteTex);

			//no textures? use white instead
			glActiveTexture(GL_TEXTURE3);
			if(currMat->map_d)
				glBindTexture(GL_TEXTURE_2D, currMat->map_d);
			else
				glBindTexture(GL_TEXTURE_2D, whiteTex);

			//no bump map? use 127,255,127 instead (0,1,0)
			glActiveTexture(GL_TEXTURE4);
			if(currMat->map_bump)
				glBindTexture(GL_TEXTURE_2D, currMat->map_bump);
			else
				glBindTexture(GL_TEXTURE_2D, blueTex);

			//std::cout<< mats->second << " ";
			if(mats + 1 != mesh->matCalls.end()){
				glDrawElements(GL_TRIANGLES, (mats+1)->second - mats->second, GL_UNSIGNED_INT, (void*)(sizeof(unsigned int) *mats->second));
				//	std::cout<<(mats+1)->second - mats->second<< " ";
			}
			else{
				glDrawElements(GL_TRIANGLES, mesh->nbVertices -mats->second, GL_UNSIGNED_INT,(void*)(sizeof(unsigned int) *mats->second));
			}
		}
	}

	renderVoxel();
	glUseProgram(colorProg);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDisable(GL_DEPTH_TEST);
	glBindVertexArray(texVAO);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, asciiTexture);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, colorTexture);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	glfwPollEvents();
	glfwSwapBuffers(window);
}
void Graphics::addStaticModel(string name){
	const ObjFile * file = gl::loadFile(name);
	for(const auto & i : file->getMeshes()){
		//Model * mod = new Model(i.second.get());
		models.push_back(std::make_unique<Model>(i.second.get()));
		models.back().get()->setScale(0.01f, 0.01,0.01);
		//std::cout<<i.first<<std::endl;
	}
}

vector<PxRigidDynamic *> Graphics::addBus(string name, Physics & physics){
	const ObjFile * file = gl::loadFile(name);
	unordered_map<string, Model*> newmods;
	for(const auto & i : file->getMeshes()){
		//Model * mod = new Model(i.second.get());
		//newmods.emplace(i.first, mod);
		models.push_back(std::make_unique<Model>(i.second.get()));
		newmods.emplace(i.first, models.back().get());
		//std::cout<<i.first<<std::endl;
	}
	//Model * mod =  new Model(loadMesh(name));
	//models.emplace_back(mods);

	name = name.substr(0, name.find_last_of('.')) + ".xml";

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
	const ObjFile * file = gl::loadFile(name);
	unordered_map<string, Model*> newmods;
	for(const auto & i : file->getMeshes()){
		//Model * mod = new Model(i.second.get());
		//newmods.emplace(i.first, mod);
		models.push_back(std::make_unique<Model>(i.second.get()));
		newmods.emplace(i.first, models.back().get());
		//std::cout<<i.first<<std::endl;
	}
	//Model * mod =  new Model(loadMesh(name));
	//models.emplace_back(mods);

	name = name.substr(0, name.find_last_of('.')) + ".xml";

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