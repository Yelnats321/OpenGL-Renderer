#pragma once

class ObjFile;

class Mesh{
	const int nbIndices;
	const ObjFile * file;
	vector<std::pair<string, int>> matCalls;
public:
	Mesh(const ObjFile * file, GLuint vao, GLuint ebo, int size, vector<std::pair<string, int> > && matCalls);
	~Mesh();
	const int getNbIndices() const;
	const ObjFile * getFile()const;
	const vector<std::pair<string,int>> & getMatCalls() const;
	GLuint vao, ebo;
};