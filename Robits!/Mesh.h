#pragma once

struct Material;

class Mesh{
	//This keeps track of when usemtl is used, and what mtl is to be used
	vector<std::pair<string, int> > materials;
	//This actually stores the material data
	map<string, Material *> matMap;
	const int size;
	const bool textures;
public:
	Mesh(GLuint, GLuint, GLuint, int, bool, vector<std::pair<string, int> > &&, map<string, Material *> &&);
	~Mesh();
	const vector<std::pair<string, int>> & getMatCalls() const;
	const Material * matData(string) const;
	const int getSize() const;
	const bool useTextures() const;
	GLuint vao, vbo, ebo;
};