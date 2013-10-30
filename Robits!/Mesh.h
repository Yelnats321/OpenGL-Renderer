#pragma once

class ObjFile;

class Mesh{
	friend class Graphics;
	const int nbIndices;
	const ObjFile * file;
	vector<std::pair<string, int>> matCalls;
	GLuint vao, ebo;
public:
	Mesh(const ObjFile * file, GLuint vao, GLuint ebo, int size, vector<std::pair<string, int> > && matCalls);
	~Mesh();
};