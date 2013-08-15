#version 330 core
layout(location = 0) in vec3 position;
uniform float screen;
out vec2 UV;
void main(){
	gl_Position = vec4(position,1);
	UV = position.xy;
}
