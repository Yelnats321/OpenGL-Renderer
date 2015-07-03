#pragma once

class ObjFile;

class Mesh{
	friend class Graphics;
	const int nbVertices;
	const ObjFile * file;
	const glm::vec3 minBounds, maxBounds;
	vector<std::pair<string, int>> matCalls;
	GLuint vao, ebo;
public:
	Mesh(const ObjFile * file, GLuint vao, GLuint ebo, int verts, vector<std::pair<string, int> > && matCalls, glm::vec3, glm::vec3);
	~Mesh();
};