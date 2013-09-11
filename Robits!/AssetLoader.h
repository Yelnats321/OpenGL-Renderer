#pragma once

class Mesh;
class ObjFile;

GLuint loadTexture(string,bool);
const ObjFile * loadFile(string);
GLuint genShaders(string, string);