#version 330 core

in vec2 UV;
layout(location = 0) out vec3 color;

uniform samplerCube passTexture;

void main(){
	//color = vec3(UV,0);
	color = vec3(texture(passTexture,vec3((UV.x)*2-1, (1-UV.y)*2-1,1)).r);
}
