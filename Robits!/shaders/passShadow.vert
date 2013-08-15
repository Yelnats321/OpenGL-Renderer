#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 position;

uniform mat4 depthMVP;
void main(){
	gl_Position = depthMVP * vec4(position,1);
}
