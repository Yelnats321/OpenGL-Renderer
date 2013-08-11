#version 330 core

in vec2 UV;
layout(location = 0) out vec3 color;

uniform sampler2D passTexture;

void main(){
	color = vec3(texture(passTexture, UV).r);
}
