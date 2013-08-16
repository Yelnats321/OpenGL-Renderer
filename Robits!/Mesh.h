#pragma once

struct Material;

class Mesh{
	//This keeps track of when usemtl is used, and what mtl is to be used
	vector<std::pair<string, int> > materials;
	//This actually stores the material data
	std::map<string, Material *> matMap;
	const int size;
public:
	Mesh(GLuint, GLuint, GLuint, int, vector<std::pair<string, int> > &&, std::map<string, Material *> &&);
	~Mesh();
	const vector<std::pair<string, int>> & getMatCalls() const;
	const Material * matData(string) const;
	const int getSize() const;
	GLuint vao, vbo, ebo;
};