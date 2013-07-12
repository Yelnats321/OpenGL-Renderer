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
	"#version 330\n"
	"layout(location = 0) in vec3 position;"
	"layout(location = 1) in vec3 normal;"
	//"layout(location = 2) in vec3 color;"
	"out vec3 Color;"
	"out vec3 NormalCamSpace;"
	"out vec3 EyeDir;"
	"out vec3 PositionWorldSpace;"
	"out vec3 LightDirCamSpace;"
	"uniform mat4 MVP;"
	"uniform mat4 M;"
	"uniform mat4 V;"
	"uniform vec3 lightPos;"
	"uniform bool wireframe;"

	"void main() {"

	"	NormalCamSpace =(V*M *vec4(normal,0.0)).xyz;"
	"	PositionWorldSpace = (M * vec4(position,1.0)).xyz;"
	"	vec3 PositionCamSpace = (V*M*vec4(position, 1.0)).xyz;"
	"	EyeDir = -PositionCamSpace;"
	"	LightDirCamSpace = (V*vec4(lightPos, 1.0)).xyz + EyeDir;"
	"	Color = vec3(0.0, 1.0, 1.0);"
	"	gl_Position = MVP * vec4(position, 1.0 );"
	"}";

const char* fragmentSource =
	"#version 330\n"

	"in vec3 Color;"
	"in vec3 NormalCamSpace;"
	"in vec3 PositionWorldSpace;"	"in vec3 EyeDir;"	"in vec3 LightDirCamSpace;"	"uniform vec3 lightPos;"	"uniform float ambientIntensity;"

	"out vec3 outColor;"

	"void main() {"
	"	vec3 n = normalize(NormalCamSpace);"
	"	vec3 l = normalize (LightDirCamSpace);"
	"	float cosTheta = clamp(dot(n, l),0,1);"

	"	vec3 E = normalize(EyeDir);"
	"	vec3 R = reflect(-l, n);"
	"	float cosAlpha = clamp(dot(E, R),0,1);"
	"	cosAlpha = pow(cosAlpha, 5);"
	"	float attenIntensity = 1.0 + 0.01*distance(lightPos, PositionWorldSpace)*distance(lightPos, PositionWorldSpace);"
	"	outColor = Color*cosTheta /attenIntensity+"
	"	Color* vec3(0.3, 0.3, 0.3) * cosAlpha/attenIntensity+"
	"	vec3(0.8, 0.8, 0.8) * ambientIntensity;"
	"}";


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
	glDeleteShader(fragShader);
	glDeleteShader(vertShader);

	for(auto i:models){
		glDeleteBuffers(1, &(i->vbo));
		glDeleteBuffers(1, &(i->ebo));
		glDeleteVertexArrays(1, &(i->vao));
		delete i;
	}
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

	GLint statusF, statusV;
	glGetShaderiv( fragShader, GL_COMPILE_STATUS, &statusF ); 
	glGetShaderiv( vertShader, GL_COMPILE_STATUS, &statusV ); 
	std::cout<<statusV<< " " << statusF << std::endl;

	// Link the vertex and fragment shader into a shader program
	shaderProg = glCreateProgram();
	glAttachShader( shaderProg, vertShader );
	glAttachShader( shaderProg, fragShader );
	glBindFragDataLocation( shaderProg, 0, "outColor" );
	glLinkProgram( shaderProg );
	glUseProgram( shaderProg );
	GLint ambInt = glGetUniformLocation(shaderProg, "ambientIntensity");	glUniform1f(ambInt, Settings::AmbientIntensity);		GLint wireframeLoc = glGetUniformLocation(shaderProg, "wireframe");	if(Settings::Wireframe){
		glUniform1i(wireframeLoc, GL_TRUE);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else{
		glUniform1i(wireframeLoc, GL_FALSE);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
}

void Graphics::update(float deltaTime){	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	input.update(deltaTime);

	GLint MVPloc = glGetUniformLocation(shaderProg, "MVP");
	GLint viewPos = glGetUniformLocation(shaderProg, "V");
	GLint viewPosInv = glGetUniformLocation(shaderProg, "Vinverse");
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
		glUniformMatrix4fv(viewPos, 1, GL_FALSE, glm::value_ptr(i->getModelMatrix()));
		glUniformMatrix4fv(viewPosInv, 1, GL_FALSE, glm::value_ptr(glm::inverse(i->getModelMatrix())));
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