#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 position;

// Output data ; will be interpolated for each fragment.
//out vec2 UV;
out vec3 pos;
uniform mat4 depthMVP;
uniform mat4 M;
void main(){
	gl_Position = depthMVP * vec4(position,1);
	pos =(M * vec4(position,1)).xyz;
}

