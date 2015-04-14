#pragma once

struct Material;
class Mesh;

class ObjFile{
	const bool textures;
	//This keeps track of when usemtl is used, and what mtl is to be used
	//This actually stores the material data
	unordered_map<string, Material> matLib;
	unordered_map<string, unique_ptr<Mesh>> meshes;
public:
	explicit ObjFile(GLuint vbo, bool useTextures, unordered_map<string, Material> && materialLibrary);
	~ObjFile();
	void addMesh(string, unique_ptr<Mesh>);
	const Material * getMaterial(string) const;
	const bool useTextures() const;
	const unordered_map<string, unique_ptr<Mesh> > & getMeshes() const;
	GLuint vbo;
};