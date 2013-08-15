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
#include "Mesh.h"

//make something similair but for models
std::map<string, GLuint> textures;

std::map<string, Mesh> meshes;

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

GLuint loadTexture(string name, bool sRGB){
	if(textures.find(name) != textures.end()){
		std::cout<<" -Returned loaded texture " +name + " at location "<<textures[name]<<std::endl;
		return textures[name];
	}
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	int width, height, channels;
	unsigned char* image = SOIL_load_image(name.c_str(), &width, &height, &channels, SOIL_LOAD_RGB);	if(sRGB)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	else
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 4.0f);
	glEnable(GL_TEXTURE_2D);
	glGenerateMipmap(GL_TEXTURE_2D);

	SOIL_free_image_data(image);
	if(image == nullptr){
		std::cout <<" ***Error loading texture " + name + " due to " + SOIL_last_result()<<std::endl;
		return 0;
	}
	std::cout << " -Loaded texture " + name + " to the location " << texture<<std::endl;
	textures[name] = texture;
	return texture;
}

void locationAppend(string origin, string * file){
	std::replace(origin.begin(), origin.end(), '/', '\\');
	if(origin.find_last_of('\\')!= string::npos){
		*file = origin.substr(0, origin.find_last_of('\\') +1) + *file;
	}
}

//remember to extract curr location 
std::map<string, Material *> loadMaterialLibrary(string name){
	std::cout << "-Loading the material library " + name<<std::endl;
	std::map<string, Material *> mats;

	std::ifstream file;
	file.open(name);
	if(file.fail()){
		std::cout << "***Error reading materials file";
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
			std::cout << " New material: "+matName<<std::endl;
			mats.emplace(std::move(matName), currMat);
		}

		else if(key == "Ns" || key == "Ni" || key == "illum" || key == "Tr" || key == "d"){
			float value = 0;
			ss >> value;

			if(key== "Ns")
				currMat->Ns = value;


			else if(key == "Ni")
				currMat->Ni = value;


			else if(key == "illum")
				currMat->illum = value;

			else 
				currMat->Tr = value;
		}

		else if(key == "Tf" || key == "Ka" || key == "Kd" || key == "Ks"){
			float a, b, c;
			a = b=c=0;
			ss >> a >> std::ws>>b>>std::ws>>c;		

			if(key == "Tf"){
				currMat->Tf[0] = a;
				currMat->Tf[1] = b;
				currMat->Tf[2] = c;
			}

			else if(key == "Ka"){
				currMat->Ka[0] = a;
				currMat->Ka[1] = b;
				currMat->Ka[2] = c;
			}

			else if(key == "Kd"){
				currMat->Kd[0] = a;
				currMat->Kd[1] = b;
				currMat->Kd[2] = c;
			}

			else{
				currMat->Ks[0] = a;
				currMat->Ks[1] = b;
				currMat->Ks[2] = c;
			}
		}

		else if(key == "map_Ka" || key == "map_Kd" || key == "map_d" || key == "bump" || key == "map_bump"){
			string mapName;
			ss>>mapName;
			locationAppend(name, &mapName);
			bool sRGB =!( key=="bump" || key == "map_bump");
			GLuint texture = loadTexture(mapName, sRGB);

			if(key == "map_Ka")
				currMat->map_Ka = texture;

			else if(key == "map_Kd")
				currMat->map_Kd = texture;

			else if(key == "map_d")
				currMat->map_d = texture;

			//if it isn't any of those maps, it must be bump map
			else
				currMat->map_bump = texture;
		}

		else{
			std::cout << itr<<std::endl;
		}
	}

	return mats;
}

struct GLuintData{
	GLuint x, y, z;
	GLuintData(GLuint a, GLuint b, GLuint c):x(a), y(b), z(c){}
	inline bool operator==(const GLuintData &rhs) const{
		return(x==rhs.x && y == rhs.y && z ==rhs.z);
	}
};

const Mesh * loadMesh(string name){
	if(meshes.find(name) != meshes.end()){
		std::cout<<"Retrieved mesh " + name <<std::endl;
		return &meshes.find(name)->second;
	}
	std::cout<<"Loading file " + name<<std::endl;
	std::ifstream file;
	file.open(name);
	if(file.fail()){
		std::cout << "Error reading file "+ name << std::endl;;
		return nullptr;
	}
	vector<string> line;
	string buf;
	while(!file.eof()){
		std::getline(file, buf);
		line.emplace_back(std::move(buf));
	}

	vector<glm::vec3> vertices;
	vector<glm::vec2> textures;
	vector<glm::vec3> normals;

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

		if(key == "v" || key == "vt" || key == "vn"){
			float x, y, z;
			ss >> x >> std::ws>> y>>std::ws>> z;
			if(key=="v")
				vertices.emplace_back(x,y,z);
			else if(key=="vt")
				textures.emplace_back(x,y);
			else
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
	bool useTextures = false, useNormals =false;

	if(pointData[0].y != 0)
		useTextures = true;
	if(pointData[0].z !=0)
		useNormals = true;
	//REMEMBER SPONGEBOB
	//RAVIOLI RAVIOLI
	//DON'T FORGET THE SUBTRACTIOLI
	//OBJ COUNTS FROM 1 NOT 0

	int size = pointData.size();

	vector<glm::vec3> tangents;
	vector<glm::vec3> bitangents;
	if(useNormals && useTextures){
		for(int i = 0; i <size; i+=3){
			//std::cout<<"triangle "<<i/3<<std::endl;
			glm::vec3 & v0 = vertices[pointData[i].x-1];
			glm::vec3 & v1 = vertices[pointData[i+1].x-1];
			glm::vec3 & v2 = vertices[pointData[i+2].x-1];

			glm::vec2 & uv0 = textures[pointData[i].y-1];
			glm::vec2 & uv1 = textures[pointData[i+1].y-1];
			glm::vec2 & uv2 = textures[pointData[i+2].y-1];

			glm::vec3 deltaPos1 = v1-v0;
			glm::vec3 deltaPos2 = v2-v0;

			glm::vec2 deltaUV1 = uv1-uv0;
			glm::vec2 deltaUV2 = uv2-uv0;

			float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
			glm::vec3 tangent = (deltaPos1 * deltaUV2.y   - deltaPos2 * deltaUV1.y)*r;
			glm::vec3 bitangent = (deltaPos2 * deltaUV1.x   - deltaPos1 * deltaUV2.x)*r;			for(int j =0; j < 3; j++){				tangents.emplace_back(tangent);				bitangents.emplace_back(bitangent);			}		}
	}
	for(int i = 0; i < pointData.size(); i++){
		glm::vec3 & n = normals[pointData[i].z-1];
		glm::vec3 & t = tangents[i];
		glm::vec3 & b = bitangents[i];

		// Gram-Schmidt orthogonalize
		t = glm::normalize(t - n * glm::dot(n, t));

		// Calculate handedness
		if (glm::dot(glm::cross(n, t), b) < 0.0f){
			t = t * -1.0f;
		}
	}
	vector<GLuint> elements(size);
	for(int i = 0; i < size; ++i)
		elements[i] = i;

	std::cout << glfwGetTime() << std::endl;
	//Assume all elements are unique, until you check out if they aren't
	for(int i =0; i<size; ++i){
			/*std::cout << tangents[i].x << " " <<tangents[i].y<<" "<<tangents[i].z<<std::endl;
			std::cout << bitangents[i].x << " " <<bitangents[i].y<<" "<<bitangents[i].z<<std::endl;
			std::cout<< normals[pointData[i].x-1].z<< " "<< normals[pointData[i].z-1].y<< " "<< normals[pointData[i].z-1].z<< std::endl<<std::endl;*/
		if(elements[i] == i){
			for(int j = i+1; j <size; ++j){
				if(pointData[i] == pointData[j]){
					//tangents[i] += tangents[j];
					//bitangents[i] += bitangents[j];
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

	int blockSize = 3+(useTextures?2:0) + (useNormals?3:0);
	if(blockSize == 8)
		blockSize = 14;

	std::vector<float> data(elemSize * blockSize, 0);
	for(int i = 0; i < elemSize; ++i){
		GLuint sElemPos = sortElem[i];
		int dataPos = i*blockSize;
		//pack all the elements that are unique
		//&vertices[pointData[elemPos]-1][0], &vertices[pointData[elemPos]][0],
		//std::copy(&vertices[pointData[elemPos]-1], &vertices[pointData[elemPos]], &data[dataPos]);
		data[dataPos] = vertices[pointData[sElemPos].x-1].x;
		data[dataPos+1] = vertices[pointData[sElemPos].x-1].y;
		data[dataPos+2] = vertices[pointData[sElemPos].x-1].z;
		if(useTextures){
			data[dataPos+3] = textures[pointData[sElemPos].y -1].x;
			data[dataPos+4] = textures[pointData[sElemPos].y -1].y;
			dataPos+=2;
		}

		if(useNormals){
			//std::copy(normals.begin() + (pointData[elemPos+2]-1)*3, normals.begin()+(pointData[elemPos+2]-1)*3+3, data.begin()+dataPos+5);
			//std::copy(&normals[(pointData[elemPos+2]-1)*3], &normals[(pointData[elemPos+2]-1)*3+3], &data[dataPos+5]);
			data[dataPos+3] = normals[pointData[sElemPos].z-1].x;
			data[dataPos+4] = normals[pointData[sElemPos].z-1].y;
			data[dataPos+5] = normals[pointData[sElemPos].z-1].z;
		}

		if(useTextures && useNormals){
			data[dataPos+6] = tangents[sElemPos].x;
			data[dataPos+7] = tangents[sElemPos].y;
			data[dataPos+8] = tangents[sElemPos].z;
			data[dataPos+ 9] = bitangents[sElemPos].x;
			data[dataPos+10] = bitangents[sElemPos].y;
			data[dataPos+11] = bitangents[sElemPos].z;
		}

		//point elements to their true location, rather than their previous pos location
		if(i != sElemPos){
			std::replace(elements.begin(), elements.end(), sElemPos, (GLuint)i);
		}
	}
	std::cout << glfwGetTime() << std::endl;
	std::cout << std::endl << elemSize << " " <<elements.size() << std::endl;
	GLuint vao = 0, vbo = 0, ebo =0;

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vbo);
	glBindBuffer( GL_ARRAY_BUFFER, vbo );
	glBufferData( GL_ARRAY_BUFFER, sizeof(float) * data.size(), &data[0], GL_STATIC_DRAW );


	glGenBuffers(1, &ebo);
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData( GL_ELEMENT_ARRAY_BUFFER, elements.size() * sizeof(GLuint), &elements[0], GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, blockSize*sizeof(float), 0);
	glEnableVertexAttribArray(0);
	if(useNormals){
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, blockSize*sizeof(float), (void*)(sizeof(float)*3));
		glEnableVertexAttribArray(1);
	}
	if(useTextures){
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, blockSize*sizeof(float), (void*)(sizeof(float)*(useNormals?5:3)));
		glEnableVertexAttribArray(2);
	}
	if(useNormals && useTextures){
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, blockSize*sizeof(float), (void*)(sizeof(float)*8));
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, blockSize*sizeof(float), (void*)(sizeof(float)*11));
		glEnableVertexAttribArray(4);
	}
	std::cout << "SIZE " << elements.size()<<std::endl;
	Mesh mesh(vao, vbo, ebo, elements.size(), std::move(materials), std::move(matLib));
	meshes.emplace(name, std::move(mesh));
	return &(meshes.find(name)->second); 
}