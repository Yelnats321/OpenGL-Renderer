#include "stdafx.h"
#include "ObjFile.h"
#include "Material.h"
#include "Mesh.h"

ObjFile::ObjFile(GLuint b, bool t, unordered_map<string, Material> && mL)
	: vbo(b), textures(t), matLib(std::move(mL)){
}

ObjFile::~ObjFile(){
	glDeleteBuffers(1, &vbo);
}

void ObjFile::addMesh(string name,  unique_ptr<Mesh> mesh){
	meshes.emplace(name, std::move(mesh));
}


const Material * ObjFile::getMaterial(string name)const{
	if(matLib.find(name) != matLib.end())
		return &matLib.find(name)->second;
	return nullptr;
}

const bool ObjFile::useTextures() const{return textures;}
const unordered_map<string, unique_ptr<Mesh>> & ObjFile::getMeshes() const{ return meshes; }