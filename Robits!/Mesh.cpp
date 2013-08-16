#include "stdafx.h"
#include "Mesh.h"
#include "Material.h"

Mesh::Mesh(GLuint a, GLuint b, GLuint e, int siz,vector<std::pair<string, int> > && m, std::map<string, Material *> && mm)
	:vao(a), vbo(b), ebo(e),size(siz),  materials(std::move(m)), matMap(std::move(mm)){
}

Mesh::~Mesh(){
	//Don't forget to delete all the textures and materials themselves
	for(auto i:matMap){
		glDeleteTextures(1,&i.second->map_Ka);
		glDeleteTextures(1,&i.second->map_Kd);
		glDeleteTextures(1,&i.second->map_d);
		glDeleteTextures(1,&i.second->map_bump);
		delete i.second;
	}
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &ebo);
	glDeleteVertexArrays(1, &vao);
}

const vector<std::pair<string, int>> & Mesh::getMatCalls() const{
	return materials;
}

const Material * Mesh::matData(string name)const{
	if(matMap.find(name) != matMap.end())
		return matMap.find(name)->second;
	return nullptr;
}

const int Mesh::getSize() const{return size;}