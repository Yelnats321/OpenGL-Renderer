#pragma once

class Mesh;

void deleteMeshes();
GLuint loadTexture(string,bool);
const Mesh * loadMesh(string);
GLuint genShaders(string, string);