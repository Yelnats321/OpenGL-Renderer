#include "AssetLoader.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>

extern "C"{
	#include <SOIL.h>
}

#include "Model.h"
#include "Material.h"

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

GLuint loadTexture(string name){
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	int width, height;
	unsigned char* image = SOIL_load_image(name.c_str(), &width, &height, 0, SOIL_LOAD_AUTO);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	SOIL_free_image_data(image);
	std::cout << "		Loaded texture " + name + " to the location " << texture<<std::endl;
	return texture;
}

void locationAppend(string origin, string * file){
	std::replace(origin.begin(), origin.end(), '\\', '/');
	if(origin.find_last_of('/')!= string::npos){
		*file = origin.substr(0, origin.find_last_of('/') +1) + *file;
	}
}

//remember to extract curr location 
std::map<string, Material *> loadMaterialLibrary(string name){
	std::cout << "	Loading the material library " + name<<std::endl;
	std::map<string, Material *> mats;

	std::ifstream file;
	file.open(name);
	if(file.fail()){
		std::cout << "	Error reading materials file " + name;
		return mats;
	}
	vector<string> line;
	string buf;
	while(!file.eof()){
		std::getline(file, buf);
		line.emplace_back(std::move(buf));
	}

	Material * currMat;

	for(auto itr:line){
		string key;
		std::istringstream ss(itr);
		ss >> std::ws>> key >> std::ws;

		if(key.length() == 0)
			continue;

		if(key.at(0) == '#')
			continue;

		if(key == "newmtl"){	
			currMat = new Material;
			string matName;
			ss >> matName;
			mats.emplace(std::move(matName), currMat);
		}

		else if(key == "Ns"){
			float Ns = 0;
			ss >> Ns;
			currMat->Ns = Ns;
		}

		else if(key == "Ni"){
			float Ni = 0;
			ss >> Ni;
			currMat->Ni = Ni;
		}

		else if(key == "Tr" || key == "d"){
			float Tr = 0;
			ss >> Tr;
			currMat->Tr = Tr;
		}

		else if(key == "Tf"){
			float a, b, c;
			a = b=c=0;
			ss >> a >> std::ws>>b>>std::ws>>c;
			currMat->Tf[0] = a;
			currMat->Tf[1] = b;
			currMat->Tf[2] = c;
		}

		else if(key == "illum"){
			float il = 0;
			ss >> il;
			currMat->illum = il;
		}

		else if(key == "Ka"){
			float a, b, c;
			a = b=c=0;
			ss >> a >> std::ws>>b>>std::ws>>c;
			currMat->Ka[0] = a;
			currMat->Ka[1] = b;
			currMat->Ka[2] = c;
		}

		else if(key == "Kd"){
			float a, b, c;
			a = b=c=0;
			ss >> a >> std::ws>>b>>std::ws>>c;
			currMat->Kd[0] = a;
			currMat->Kd[1] = b;
			currMat->Kd[2] = c;
		}

		else if(key == "Ks"){
			float a, b, c;
			a = b=c=0;
			ss >> a >> std::ws>>b>>std::ws>>c;
			currMat->Ks[0] = a;
			currMat->Ks[1] = b;
			currMat->Ks[2] = c;
		}

		else if(key == "map_Ka"){
			string mapKa;
			ss >> mapKa;
			currMat->map_Ka = mapKa;
		}

		else if(key == "map_Kd"){
			string mapKd;
			ss >> mapKd;
			locationAppend(name, &mapKd);
			currMat->map_Kd = loadTexture(mapKd);
		}

		else if(key == "map_d"){
			string mapd;
			ss >> mapd;
			currMat->map_d = mapd;
		}

		else if(key == "bump" || key == "map_bump"){
			string mapbump;
			ss >> mapbump;
			currMat->map_bump = mapbump;
		}

		else{
			//std::cout << itr;
		}
	}

	return mats;
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

Model * loadModel(string name){
	std::cout<<"Loading file " + name<<std::endl;
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
	vector<GLuintData> pointData;
	vector<std::pair<std::string, int> > materials;
	std::map<string, Material *>matLib;
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
			float x, y;
			ss >> x >> std::ws>> y;
			textures.emplace_back(x,y,1.f);
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
			locationAppend(name, &matName);

			matLib = std::move(loadMaterialLibrary(matName));
		}

		else if(key == "usemtl"){
			string mtlName;
			ss >> mtlName;
			materials.emplace_back(std::move(mtlName), pointData.size()/3);
		}
		//faces
		else if(key == "f"){
			for(int i = 0; i < 3; ++i){
				GLuint a,b,c;
				a=b=c=0;

				ss >> a;
				if(ss.get() == '/'){
					if(ss.peek()!='/'){
						ss >> b;
					}
					if(ss.get() == '/'){
						ss>> c;
					}
				}
				pointData.emplace_back(a,b,c);
				if(i ==2){
					ss>>std::ws;
					if(!ss.eof()){
						pointData.emplace_back(a,b,c);
						a=b=c=0;

						ss >> a;
						if(ss.get() == '/'){
							if(ss.peek()!='/'){
								ss >> b;
							}
							if(ss.get() == '/'){
								ss>> c;
							}
						}	

						pointData.emplace_back(a,b,c);

						pointData.emplace_back(pointData[pointData.size()-5]);
					}
				}
			}
		}
	}
	int size = pointData.size();
	vector<GLuint> elements(size);
	for(int i = 0; i < size; ++i)
		elements[i] = i;

	std::cout << glfwGetTime() << std::endl;
	//Assume all elements are unique, until you check out if they aren't
	for(int i =0; i<size; ++i){
		if(elements[i] == i){
			for(int j = i+1; j <size; ++j){
				if(pointData[i] == pointData[j]){
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
		GLuint elemPos = sortElem[i];
		int dataPos = i*8;
		//pack all the elements that are unique
		//&vertices[pointData[elemPos]-1][0], &vertices[pointData[elemPos]][0],
		//std::copy(&vertices[pointData[elemPos]-1], &vertices[pointData[elemPos]], &data[dataPos]);
		data[dataPos] = vertices[pointData[elemPos].x-1].x;
		data[dataPos+1] = vertices[pointData[elemPos].x-1].y;
		data[dataPos+2] = vertices[pointData[elemPos].x-1].z;
		if(pointData[elemPos].y != 0){
			data[dataPos+3] = textures[pointData[elemPos].y -1].x;
			data[dataPos+4] = textures[pointData[elemPos].y -1].y;
		}
		else{
			data[dataPos+3] = 0;
			data[dataPos+4] = 0;
		}

		if(pointData[elemPos].z != 0){
			//std::copy(normals.begin() + (pointData[elemPos+2]-1)*3, normals.begin()+(pointData[elemPos+2]-1)*3+3, data.begin()+dataPos+5);
			//std::copy(&normals[(pointData[elemPos+2]-1)*3], &normals[(pointData[elemPos+2]-1)*3+3], &data[dataPos+5]);
			data[dataPos+5] = normals[pointData[elemPos].z-1].x;
			data[dataPos+6] = normals[pointData[elemPos].z-1].y;
			data[dataPos+7] = normals[pointData[elemPos].z-1].z;
		}
		else{
			data[dataPos+5] = 1;
			data[dataPos+6] = 0;
			data[dataPos+7] = 0;
		}
		//point elements to their true location, rather than their previous pos location
		if(i != elemPos){
			std::replace(elements.begin(), elements.end(), elemPos, (GLuint)i);
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
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(sizeof(float)*3));
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(sizeof(float)*5));

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	glBindVertexArray(0);
	Model * mod = new Model(vao, vbo, ebo, elements.size(), std::move(materials), std::move(matLib));
	return mod;
}