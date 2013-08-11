#pragma once
#include "Graphics.h"
#include <map>

struct Material;

GLuint loadTexture(string,bool);
std::map<string, Material *> loadMaterialLibrary(string);
Model * loadModel(string);
GLuint genShaders(string, string);