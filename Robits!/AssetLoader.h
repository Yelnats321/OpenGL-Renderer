#pragma once
#include <map>
#include <string>
//glew needs to be loaded before GLFW because of gl.h
#include <GL/glew.h>
#include <GLFW/glfw3.h>

using std::string;
struct Material;
class Model;
class Mesh;

GLuint loadTexture(string,bool);
const Mesh * loadMesh(string);
GLuint genShaders(string, string);