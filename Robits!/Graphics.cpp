#include "Graphics.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>

#include "Settings.h"
#include "Model.h"
#include "Player.h"
#include "Material.h"

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

GLuint genShaders(string vert, string frag){
	std::ifstream file;

	string vertexSource, fragmentSource;
	file.open(vert);
	if(file){
		file.seekg(0, std::ios::end);
		vertexSource.resize(file.tellg());
		file.seekg(0, std::ios::beg);
		file.read(&vertexSource[0], vertexSource.size());
		file.close();
	}
	file.open(frag);
	if(file){
		file.seekg(0, std::ios::end);
		fragmentSource.resize(file.tellg());
		file.seekg(0, std::ios::beg);
		file.read(&fragmentSource[0], fragmentSource.size());
		file.close();
	}

	// Create and compile the vertex shader
	GLuint vertShader = glCreateShader( GL_VERTEX_SHADER );
	const char *c_str1 = vertexSource.c_str();
	glShaderSource( vertShader, 1, &c_str1, NULL );
	glCompileShader( vertShader );

	// Create and compile the fragment shader
	GLuint fragShader = glCreateShader( GL_FRAGMENT_SHADER );
	const char *c_str2 = fragmentSource.c_str();
	glShaderSource( fragShader, 1, &c_str2, NULL );
	glCompileShader( fragShader );

	GLint statusF, statusV;
	glGetShaderiv( fragShader, GL_COMPILE_STATUS, &statusF ); 
	glGetShaderiv( vertShader, GL_COMPILE_STATUS, &statusV ); 
	std::cout<<statusV<< " " << statusF << std::endl;

	// Link the vertex and fragment shader into a shader program
	GLuint shaderProg = glCreateProgram();
	glAttachShader( shaderProg, vertShader );
	glAttachShader( shaderProg, fragShader );
	glLinkProgram( shaderProg );
	glDetachShader(shaderProg, vertShader);
	glDetachShader(shaderProg, fragShader);
	glDeleteShader(vertShader);
	glDeleteShader(fragShader);

	return shaderProg;

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
	glUniform3fv(lightPos, 1, glm::value_ptr(player->getPos()));
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
		glDrawElements( GL_TRIANGLES,i->triangles * 3, GL_UNSIGNED_INT,0);
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

void Graphics::setPlayer(Player * p){
	player = p;
	input.setPlayer(p);
}
//remember to extract curr location 
void loadMaterials(string name){
	std::ifstream file;
	file.open(name);
	if(file.fail()){
		std::cout << "Error reading materials file " + name;
		return;
	}
	vector<string> line;
	string buf;
	while(!file.eof()){
		std::getline(file, buf);
		line.emplace_back(std::move(buf));
	}

	//Material * currMat;

	for(auto itr:line){
		string key;
		std::istringstream ss(itr);
		ss >> std::ws>> key >> std::ws;

		if(key.length() == 0)
			continue;

		if(key.at(0) == '#')
			continue;

		if(key == "newmtl"){
		}
	}
}

template <typename T>
struct threeData{
	T x, y, z;
	threeData(T a, T b, T c):x(a), y(b), z(c){}
	bool operator==(const threeData &other) const{
		return(x==other.x && y == other.y && z ==other.z);
	}
};

typedef threeData<float> floatData;
typedef threeData<GLuint> GLuintData;

Model * Graphics::loadModel(string name){
	std::ifstream file;
	file.open(name);
	if(file.fail()){
		std::cout << "Error reading file "+ name;
		return nullptr;
	}
	vector<string> line;
	string buf;
	while(!file.eof()){
		std::getline(file, buf);
		line.emplace_back(std::move(buf));
	}

	vector<floatData> vertices;
	vector<floatData> textures;
	vector<floatData> normals;
	vector<GLuintData> triangleData;
	vector<std::pair<std::string, int> > materials;
	for(auto itr : line){
		string key;
		std::istringstream ss(itr);
		ss >> std::ws>> key >> std::ws;

		if(key.length() == 0)
			continue;

		if(key.at(0) == '#')
			continue;

		//vertecies
		if(key == "v"){
			float x, y, z;
			ss >> x >> std::ws>> y>>std::ws>> z;
			vertices.emplace_back(x,y,z);
		}

		//textures
		else if(key == "vt"){
			float x, y, z;
			ss >> x >> std::ws>> y>>std::ws>> z;
			textures.emplace_back(x,y,z);
		}

		//normals
		else if(key == "vn"){	
			float x, y, z;
			ss >> x >> std::ws>> y>>std::ws>> z;
			normals.emplace_back(x,y,z);
		}

		else if(key == "mtllib"){
			string matName;
			ss >> matName;
			if(name.find_last_of('/')!= string::npos)
				matName = name.substr(0, name.find_last_of('/')+1) + matName;
			loadMaterials(matName);
		}

		else if(key == "usemtl"){
			materials.emplace_back(itr, triangleData.size()/3);
		}
		//faces
		else if(key == "f"){			for(int i = 0; i < 3; ++i){				GLuint a,b,c;				a=b=c=0;				ss >> a;				if(ss.get() == '/'){					if(ss.peek()!='/'){						ss >> b;					}					if(ss.get() == '/'){						ss>> c;					}				}				triangleData.emplace_back(a,b,c);				if(i ==2){					ss>>std::ws;					if(!ss.eof()){						triangleData.emplace_back(a,b,c);						a=b=c=0;						ss >> a;						if(ss.get() == '/'){							if(ss.peek()!='/'){								ss >> b;							}							if(ss.get() == '/'){								ss>> c;							}						}							triangleData.emplace_back(a,b,c);						triangleData.emplace_back(triangleData[triangleData.size()-5]);					}				}
			}
		}
	}
	int size = triangleData.size();
	vector<GLuint> elements(size);
	for(int i = 0; i < size; ++i)
		elements[i] = i;

	std::cout << glfwGetTime() << std::endl;
	//Assume all elements are unique, until you check out if they aren't
	for(int i =0; i<size; ++i){
		if(elements[i] == i){
			for(int j = i+1; j <size; ++j){
				if(triangleData[i] == triangleData[j]){
						elements[j] = i;
				}
			}
		}
	}

	//these are just the elements, since they kept their position as their value, they have to be compressed
	vector<GLuint> sortElem(elements);
	std::sort(sortElem.begin(), sortElem.end());
	sortElem.resize(std::distance(sortElem.begin(), std::unique(sortElem.begin(), sortElem.end())));

	std::cout << glfwGetTime() << std::endl;
	int elemSize = sortElem.size();
	std::vector<float> data(elemSize *8, 0);
	for(int i = 0; i < elemSize; ++i){
		int elemPos = sortElem[i];
		int dataPos = i*8;
		//pack all the elements that are unique//&vertices[triangleData[elemPos]-1][0], &vertices[triangleData[elemPos]][0],
		//std::copy(&vertices[triangleData[elemPos]-1], &vertices[triangleData[elemPos]], &data[dataPos]);
		data[dataPos] = vertices[triangleData[elemPos].x-1].x;
		data[dataPos+1] = vertices[triangleData[elemPos].x-1].y;
		data[dataPos+2] = vertices[triangleData[elemPos].x-1].z;
		//textures, unused right now
		/*if(triangleData[sortElem[i]*3+1] != 0){
		data[i*8+3] = textures[(triangleData[elemPos+1] -1)*2];
		data[i*8+4] = textures[(triangleData[elemPos+1] -1)*2+1];
		}
		else*/{
			data[dataPos+3] = 0;
			data[dataPos+4] = 0;
		}

		if(triangleData[sortElem[i]].z != 0){			//std::copy(normals.begin() + (triangleData[elemPos+2]-1)*3, normals.begin()+(triangleData[elemPos+2]-1)*3+3, data.begin()+dataPos+5);
			//std::copy(&normals[(triangleData[elemPos+2]-1)*3], &normals[(triangleData[elemPos+2]-1)*3+3], &data[dataPos+5]);
			data[dataPos+5] = normals[triangleData[elemPos].z-1].x;
			data[dataPos+6] = normals[triangleData[elemPos].z-1].y;
			data[dataPos+7] = normals[triangleData[elemPos].z-1].z;
		}
		else{
			data[dataPos+5] = 0.3;
			data[dataPos+6] = 0.3;
			data[dataPos+7] = 0.3;
		}
		//point elements to their true location, rather than their previous pos location
		if(i != sortElem[i]){
			std::replace(elements.begin(), elements.end(), sortElem[i], (GLuint)i);
		}
	}
	std::cout << glfwGetTime() << std::endl;
	std::cout << std::endl << elemSize << " " <<elements.size() << std::endl;
	GLuint vao = 0, vbo = 0, ebo =0;

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vbo);
	glBindBuffer( GL_ARRAY_BUFFER, vbo );
	glBufferData( GL_ARRAY_BUFFER, sizeof(float) * elemSize *8, &data[0], GL_STATIC_DRAW );


	glGenBuffers(1, &ebo);
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData( GL_ELEMENT_ARRAY_BUFFER, elements.size() * sizeof(GLuint), &elements[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(sizeof(float)*5));

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	Model * mod = new Model(vao, vbo, ebo, elements.size(), std::move(materials));
	models.push_back(mod);
	return mod;
}

bool Graphics::isOpen() const{
	return !glfwWindowShouldClose(window);
}

GLFWwindow * Graphics::getWindow() const{
	return window;
}