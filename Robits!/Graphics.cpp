#include "Graphics.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>

#include "Settings.h"
#include "Model.h"
#include "Player.h"

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

	if(!Settings::Wireframe)
		glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	glClearColor( 0.4f, 0.2f, 0.3f, 1.0f );

	// Create Vertex Array Object

	proj = glm::perspective( 50.0f, 800.0f / 600.0f, 0.01f, 10000.0f );
}

Graphics::~Graphics(){
	glDeleteProgram(shaderProg);

	for(auto i:models){
		glDeleteBuffers(1, &(i->vbo));
		glDeleteBuffers(1, &(i->ebo));
		glDeleteVertexArrays(1, &(i->vao));
		delete i;
	}
	glfwDestroyWindow(window);
    glfwTerminate();
}

void Graphics::genShaders(string vert, string frag){
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
	shaderProg = glCreateProgram();
	glAttachShader( shaderProg, vertShader );
	glAttachShader( shaderProg, fragShader );
	glLinkProgram( shaderProg );
	glDetachShader(shaderProg, vertShader);
	glDetachShader(shaderProg, fragShader);
	glDeleteShader(vertShader);
	glDeleteShader(fragShader);
	glUseProgram( shaderProg );


	GLint ambInt = glGetUniformLocation(shaderProg, "ambientIntensity");
	glUniform1f(ambInt, Settings::AmbientIntensity);
	
	GLint wireframeLoc = glGetUniformLocation(shaderProg, "wireframe");
	if(Settings::Wireframe){
		glUniform1i(wireframeLoc, GL_TRUE);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else{
		glUniform1i(wireframeLoc, GL_FALSE);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
}


void Graphics::genBuffers(){
	/*GLuint FramebufferName = 0;
	glGenFramebuffers(1, &FramebufferName);
	glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);

	GLuint renderedTexture;
	glGenTextures(1, &renderedTexture);

	// "Bind" the newly created texture : all future texture functions will modify this texture
	glBindTexture(GL_TEXTURE_2D, renderedTexture);

	// Give an empty image to OpenGL ( the last "0" )
	glTexImage2D(GL_TEXTURE_2D, 0,GL_RGB, 800, 600, 0,GL_RGB, GL_UNSIGNED_BYTE, 0);

	// Poor filtering. Needed !
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	GLuint depthrenderbuffer;
	glGenRenderbuffers(1, &depthrenderbuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, depthrenderbuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, 800, 600);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthrenderbuffer);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, renderedTexture, 0);

	// Set the list of draw buffers.
	GLenum DrawBuffers[2] = {GL_COLOR_ATTACHMENT0};
	glDrawBuffers(1, DrawBuffers);*/
}

void Graphics::update(float deltaTime){	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	input.update(deltaTime);

	GLint MVPloc = glGetUniformLocation(shaderProg, "MVP");
	GLint viewPos = glGetUniformLocation(shaderProg, "V");
	GLint modelPos = glGetUniformLocation(shaderProg, "M");
	GLint lightPos = glGetUniformLocation(shaderProg, "lightPos");
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
	glfwPollEvents();
	glfwSwapBuffers(window);
}

void Graphics::setPlayer(Player * p){
	player = p;
	input.setPlayer(p);
}

Model * Graphics::loadModel(string name){
	std::ifstream file;
	file.open(name);
	if(file.fail()){
		std::cout << "Error reading file";
		return nullptr;
	}
	vector<string> line;
	string buf;
	while(!file.eof()){
		std::getline(file, buf);
		line.emplace_back(std::move(buf));
	}

	vector<float> vertices;
	vector<float> textures;
	vector<float> normals;
	vector<GLuint> triangleData;
	vector<std::pair<std::string, int> > materials;
	for(auto itr : line){
		//comments
		if(itr.length() == 0)
			continue;

		string key;
		std::istringstream ss(itr);
		ss >> key >> std::ws;

		if(key.at(0) == '#')
			continue;

		//vertecies
		if(key == "v"){
			float x, y, z;
			ss >> x >> std::ws>> y>>std::ws>> z;
			vertices.push_back(x);
			vertices.push_back(y);
			vertices.push_back(z);
		}

		//textures
		else if(key == "vt"){
			continue;
		}

		//normals
		else if(key == "vn"){	
			float x, y, z;
			ss >> x >> std::ws>> y>>std::ws>> z;
			normals.push_back(x);
			normals.push_back(y);
			normals.push_back(z);
		}

		else if(key == "mtllib"){
		}

		else if(key == "usemtl"){
			materials.emplace_back(itr, triangleData.size()/3);
		}
		//faces
		else if(key == "f"){			for(int i = 0; i < 3; i++){				int a,b,c;				a=b=c=0;				ss >> a;				if(ss.get() == '/'){					if(ss.peek()!='/'){						ss >> b;					}					if(ss.get() == '/'){						ss>> c;					}				}				triangleData.push_back(a);				triangleData.push_back(b);				triangleData.push_back(c);				if(i ==2){					ss>>std::ws;					if(!ss.eof()){						triangleData.push_back(a);						triangleData.push_back(b);						triangleData.push_back(c);						a=b=c=0;						ss >> a;						if(ss.get() == '/'){							if(ss.peek()!='/'){								ss >> b;							}							if(ss.get() == '/'){								ss>> c;							}						}							triangleData.push_back(a);						triangleData.push_back(b);						triangleData.push_back(c);						int size = triangleData.size();						a = triangleData[size-12-3];						b = triangleData[size-12-2];						c = triangleData[size-12-1];						triangleData.push_back(a);						triangleData.push_back(b);						triangleData.push_back(c);					}				}
			}
		}
	}
	int size = triangleData.size()/3;
	vector<GLuint> elements(size);
	for(int i = 0; i < size; i++)
		elements[i] = i;

	std::cout << glfwGetTime() << std::endl;
	//Assume all elements are unique, until you check out if they aren't
	for(int i =0; i<size; i++){
		if(elements[i] == i){
			for(int j = i+1; j <size; j++){
				if(triangleData[i*3] == triangleData[j*3] &&
					triangleData[i*3+1] == triangleData[j*3+1] &&
					triangleData[i*3+2] == triangleData[j*3+2]){
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
	for(int i = 0; i < elemSize; i++){
		int elemPos = sortElem[i]*3;
		int dataPos = i*8;
		//pack all the elements that are unique
		std::copy(vertices.begin() + (triangleData[elemPos]-1)*3, vertices.begin() + (triangleData[elemPos]-1)*3+3, data.begin() +dataPos);
		//textures, unused right now
		/*if(triangleData[sortElem[i]*3+1] != 0){
		data[i*8+3] = textures[(triangleData[elemPos+1] -1)*2];
		data[i*8+4] = textures[(triangleData[elemPos+1] -1)*2+1];
		}
		else*/{
			data[dataPos+3] = 0;
			data[dataPos+4] = 0;
		}

		if(triangleData[sortElem[i]*3+2] != 0){
			std::copy(normals.begin() + (triangleData[elemPos+2]-1)*3, normals.begin()+(triangleData[elemPos+2]-1)*3+3, data.begin()+dataPos+5);
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