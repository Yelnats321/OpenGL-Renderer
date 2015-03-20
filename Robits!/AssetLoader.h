#pragma once

class Mesh;
class ObjFile;

GLuint loadTexture(string, bool = false , bool=true);
const ObjFile * loadFile(string);
GLuint genShaders(string, string);