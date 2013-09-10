#pragma once

struct Material;
class Mesh;

class ObjFile{
	const bool textures;
	//This keeps track of when usemtl is used, and what mtl is to be used
	//This actually stores the material data
	map<string, Material *> matLib;
	map<string, Mesh *> meshes;
public:
	ObjFile(GLuint vbo, bool useTextures, map<string, Material *> && materialLibrary);
	~ObjFile();
	void addMesh(string, Mesh*);
	const Material * getMaterial(string) const;
	const bool useTextures() const;
	const map<string,  Mesh*> & getMeshes() const;
	GLuint vbo;
};