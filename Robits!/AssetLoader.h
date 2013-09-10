#pragma once

class Mesh;
class ObjFile;

void deleteFiles();
GLuint loadTexture(string,bool);
const ObjFile * loadFile(string);
GLuint genShaders(string, string);