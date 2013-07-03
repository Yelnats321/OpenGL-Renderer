#include "Graphics.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>

#include "Settings.h"
#include "Model.h"
#include "Player.h"

// Shader sources
const char* vertexSource =
	"#version 150\n"
	"in vec3 position;"
	"in vec3 color;"
	"uniform mat4 MVP;"
	"uniform float currTime;"
	"uniform bool wireframe;"

	"out vec3 Color;"
	"void main() {"
	"	vec3 npos = position;"
	"	if(wireframe){"
	"		Color = vec3(0.0, 1.0, 1.0);"
	//"		npos= npos*1.01;"
	"	}"
	"	else"
	"		Color = color;"
	"	gl_Position = MVP * vec4(npos, 1.0 );"
	"}";

const char* fragmentSource =
	"#version 150\n"
	"in vec3 Color;"
	"out vec4 outColor;"
	"void main() {"
	"	outColor = vec4(Color, 1.0 );"
	"}";


Graphics::Graphics(){    glfwInit();

	glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 3 );
	glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 2 );
	glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );

	glfwWindowHint( GLFW_RESIZABLE, GL_FALSE );
	window = glfwCreateWindow(800, 600, "OpenGL Window", 0, 0);

	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = GL_TRUE;
	glewInit();

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	//glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	glClearColor( 0.4f, 0.2f, 0.3f, 1.0f );

	// Create Vertex Array Object
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
}

Graphics::~Graphics(){
	glDeleteProgram(shaderProg);
	glDeleteShader(fragShader);
	glDeleteShader(vertShader);

	for(auto i:models){
		glDeleteBuffers(1, &(i->vbo));
		glDeleteBuffers(1, &(i->ebo));
		delete i;
	}
	glDeleteVertexArrays(1, &vao);
	glfwDestroyWindow(window);
    glfwTerminate();
}

void Graphics::genShaders(){
	// Create and compile the vertex shader
	vertShader = glCreateShader( GL_VERTEX_SHADER );
	glShaderSource( vertShader, 1, &vertexSource, NULL );
	glCompileShader( vertShader );

	// Create and compile the fragment shader
	fragShader = glCreateShader( GL_FRAGMENT_SHADER );
	glShaderSource( fragShader, 1, &fragmentSource, NULL );
	glCompileShader( fragShader );

	GLint status;
	glGetShaderiv( fragShader, GL_COMPILE_STATUS, &status ); 
	std::cout<<status<< " " << GL_TRUE << std::endl;

	// Link the vertex and fragment shader into a shader program
	shaderProg = glCreateProgram();
	glAttachShader( shaderProg, vertShader );
	glAttachShader( shaderProg, fragShader );
	glBindFragDataLocation( shaderProg, 0, "outColor" );
	glLinkProgram( shaderProg );
	glUseProgram( shaderProg );

	proj = glm::perspective( 50.0f, 800.0f / 600.0f, 0.00001f, 10.0f );

	currTimeLoc = glGetUniformLocation(shaderProg,"currTime");
	wireframeLoc = glGetUniformLocation(shaderProg, "wireframe");

	posAttrib = glGetAttribLocation( shaderProg, "position" );
	glEnableVertexAttribArray( posAttrib );

	colAttrib = glGetAttribLocation( shaderProg, "color" );
	glEnableVertexAttribArray( colAttrib );
}

void Graphics::update(float deltaTime) const{	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, 1);

	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);
	glfwSetCursorPos(window, 800.0/2, 600.0/2);

	player->update(float(800/2 - xpos), float(600/2 - ypos), deltaTime);

	GLint MVPloc = glGetUniformLocation( shaderProg, "MVP" );
	glUniform1f(currTimeLoc, glfwGetTime());

	/*camera = glm::lookAt(
	position,
	position + glm::vec3(0.0f, -1.0f, -1.0f),
	glm::vec3(0.f, 1.f, 0.0f)
	);*/
	
	if(Settings::Wireframe){
		glUniform1i(wireframeLoc, GL_TRUE);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else{
		glUniform1i(wireframeLoc, GL_FALSE);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	for(auto i : models){
		glm::mat4 matrix = proj * player->getCameraMatrix() * i->getModelMatrix();
		/*std::cout << matrix[0][0] << " " << matrix[0][1] << " " << matrix[0][2] << " " <<matrix[0][3] << std::endl;
		std::cout << matrix[1][0] << " " << matrix[1][1] << " " << matrix[1][2] << " " <<matrix[1][3] << std::endl;
		std::cout << matrix[2][0] << " " << matrix[2][1] << " " << matrix[2][2] << " " <<matrix[2][3] << std::endl;
		std::cout << matrix[3][0] << " " << matrix[3][1] << " " << matrix[3][2] << " " <<matrix[3][3] << std::endl;
		std::cout << std::endl;*/
		glUniformMatrix4fv(MVPloc, 1, GL_FALSE, glm::value_ptr(matrix));
		glBindBuffer(GL_ARRAY_BUFFER, i->vbo);
		glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), 0);
		glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(sizeof(float)*5));		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, i->ebo);
		glDrawElements( GL_TRIANGLES,i->triangles * 3, GL_UNSIGNED_SHORT,0);
	}
	glfwPollEvents();
	glfwSwapBuffers(window);
}

void Graphics::setPlayer(Player * p){
	player = p;
}

Model * Graphics::loadModel(string name){
	std::ifstream file;
	file.open(name);
	vector<string> line;
	char buf[256];
	while(!file.eof()){
		file.getline(buf, 256);
		line.push_back(buf);
	}

	vector<glm::vec3> vertices;
	vector<glm::vec2> textures;
	vector<glm::vec3> normals;
	vector<std::array<GLushort, 3> > triangleData;

	for(auto i : line){
		//comments
		if(i.length() == 0)
			continue;
		if(i.at(0) == '#')
			continue;

		//vertecies
		if(i.at(0) == 'v' && i.at(1) == ' '){
			string x, y, z;
			std::istringstream ss(i);
			ss.ignore(2, ' ');
			ss >> x;
			ss >> y;
			ss >> z;
			vertices.push_back(glm::vec3(atof(x.c_str()),atof(y.c_str()),atof(z.c_str())));
		}

		//textures
		else if(i.at(0) == 'v' && i.at(1) == 't'){
		}

		//normals
		else if(i.at(0) == 'v' && i.at(1) == 'n'){	
			string x, y, z;
			std::istringstream ss(i);
			ss.ignore(3, ' ');
			ss >> x;
			ss >> y;
			ss >> z;
			normals.push_back(glm::vec3(atof(x.c_str()),atof(y.c_str()),atof(z.c_str())));
		}

		//faces
		else if(i.at(0) == 'f'){			string o[3];//o for original(data)
			std::istringstream ss(i);
			ss.ignore(2, ' ');
			for(int i = 0; i < 3; i++)
				ss >> o[i];

			//v for vertex
			//n for normal
			if(o[0].find('/') == string::npos){
				int v[3];
				for(int i = 0; i <3; i++){
					v[i] = ::atoi(o[i].c_str());
					std::array<GLushort, 3> temp = {v[i],0,0};
					triangleData.push_back(temp);
				}
			}
			//if there is one occurence of '/', and another right after
			if(o[0].find("//") != string::npos){
				int v[3], n[3];
				for(int i = 0; i <3; i++){
					v[i] = ::atoi(o[i].substr(0, o[i].find("//")).c_str());
					n[i] = ::atoi(o[i].substr(o[i].find("//")+2).c_str());
					std::array<GLushort, 3> temp = {v[i], 0, n[i]};
					triangleData.push_back(temp);
				}
			}
		}
	}
	vector<GLushort> elements(triangleData.size());
	for(int i = 0; i < triangleData.size(); i++)
		elements[i] = i;
	//Assume all elements are unique, until you check out if they aren't
	for(int i =0; i<elements.size(); i++){
		if(elements[i] != i)
			continue;

		for(int j = i+1; j <elements.size(); j++){
			if(triangleData[i][0] == triangleData[j][0] &&
				triangleData[i][1] == triangleData[j][1] &&
				triangleData[i][2] == triangleData[j][2]){
				elements[j] = i;
			}
		}
	}
	//these are just the elements, since they kept their position as their value, they have to be compressed
	vector<GLushort> sortElem(elements);
	std::sort(sortElem.begin(), sortElem.end());
	sortElem.resize(std::distance(sortElem.begin(), std::unique(sortElem.begin(), sortElem.end())));
	
	std::vector<float> data;
	data.clear();
	data.resize(sortElem.size() *8, 0);
	for(int i = 0; i < sortElem.size(); i++){
		//pack all the elements that are unique
		std::array<GLushort, 3> triData = triangleData[sortElem[i]];

		data[i*8] = vertices[triData[0]-1].x;
		data[i*8+1] = vertices[triData[0]-1].y;
		data[i*8+2] = vertices[triData[0]-1].z;
		//textures, unused right now
		if(triData[1] != 0){
			data[i*8+3] = textures[triData[2] -1].x;
			data[i*8+4] = textures[triData[2] -1].y;
		}
		else{
			data[i*8+3] = 0;
			data[i*8+4] = 0;
		}

		if(triData[2] != 0){
			data[i*8+5] = normals[triData[2] -1].x;
			data[i*8+6] = normals[triData[2] -1].y;
			data[i*8+7] = normals[triData[2] -1].z;
		}
		else{
			data[i*8+5] = 0.3;
			data[i*8+6] = 0.3;
			data[i*8+7] = 0.3;
		}
		//point elements to their true location, rather than their previous pos location
		if(i != sortElem[i]){
			std::replace(elements.begin(), elements.end(), sortElem[i], (unsigned short)i);
		}
	}
	std::cout << std::endl << sortElem.size() << " " <<elements.size() << std::endl;
	//float * data = new float[std::unique(elements.begin(), elements.end())];
	GLuint vbo = 0, ebo =0;

	glGenBuffers(1, &vbo);
	glBindBuffer( GL_ARRAY_BUFFER, vbo );
	glBufferData( GL_ARRAY_BUFFER, sizeof(float) * sortElem.size()*8, &data[0], GL_STATIC_DRAW );
	//glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), 0);
	//glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(sizeof(float)*5));

	//glGenBuffers(1, &vboT);
	//glBindBuffer(GL_ARRAY_BUFFER, vboT);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * textures.size(), &textures[0], GL_STATIC_DRAW);
	//glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), 0);

	//glGenBuffers(1, &vboN);
	//glBindBuffer(GL_ARRAY_BUFFER, vboN);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * normals.size(), &normals[0], GL_STATIC_DRAW);
	//glVertexAttribPointer(normAttrib, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), 0);

	glGenBuffers(1, &ebo);
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData( GL_ELEMENT_ARRAY_BUFFER, elements.size() * sizeof(GLushort), &elements[0], GL_STATIC_DRAW);
	Model * mod = new Model(vbo, ebo, elements.size());
	models.push_back(mod);
	return mod;
}

bool Graphics::isOpen() const{
	return !glfwWindowShouldClose(window);
}

GLFWwindow * Graphics::getWindow() const{
	return window;
}