#include "stdafx.h"
#include "ObjFile.h"
#include "Material.h"

ObjFile::ObjFile( GLuint b, bool t, map<string, Material *> && mL)
	: vbo(b), textures(t), matLib(std::move(mL)){
}

ObjFile::~ObjFile(){
	//Don't forget to delete all the textures and materials themselves
	for(const auto & i:matLib){
		glDeleteTextures(1,&i.second->map_Ka);
		glDeleteTextures(1,&i.second->map_Kd);
		glDeleteTextures(1,&i.second->map_d);
		glDeleteTextures(1,&i.second->map_bump);
		delete i.second;
	}
	matLib.clear();
	for(const auto & i:meshes){
		delete i.second;
	}
	meshes.clear();
	glDeleteBuffers(1, &vbo);
}

void ObjFile::addMesh(string name,  Mesh * mesh){
	meshes.emplace(name, mesh);
}


const Material * ObjFile::getMaterial(string name)const{
	if(matLib.find(name) != matLib.end())
		return matLib.find(name)->second;
	return nullptr;
}

const bool ObjFile::useTextures() const{return textures;}
const map<string, Mesh*> & ObjFile::getMeshes() const{return meshes;}