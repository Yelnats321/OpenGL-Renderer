#pragma once
#include "Graphics.h"
#include <map>

struct Material;

GLuint loadTexture(string);
std::map<string, Material *> loadMaterialLibrary(string);
Model * loadModel(string);
GLuint genShaders(string, string);