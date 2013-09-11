#include "stdafx.h"
#include "Mesh.h"

Mesh::Mesh(const ObjFile * f, GLuint a, GLuint e, int siz,vector<std::pair<string, int> > && m)
	:file(f),vao(a), ebo(e), nbIndices(siz),matCalls(std::move(m)){
}

Mesh::~Mesh(){
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &ebo);
}

//size is number of TRIANGLES POINTS IE INDICES
const int Mesh::getNbIndices() const{return nbIndices;}
const ObjFile * Mesh::getFile() const{return file;}
const vector<std::pair<string,int>> & Mesh::getMatCalls() const{return matCalls;}