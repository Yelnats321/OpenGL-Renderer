#version 330

layout(points) in;
layout(line_strip, max_vertices=17) out;

uniform mat4 VP;

const float size = 1;

void main(){
	gl_Position = VP* (gl_in[0].gl_Position + vec4(size/2,size/2, size/2,0));
	EmitVertex();
	gl_Position = VP* (gl_in[0].gl_Position + vec4(size/2,size/2, -size/2,0));
	EmitVertex();
	gl_Position = VP* (gl_in[0].gl_Position + vec4(size/2,-size/2, -size/2,0));
	EmitVertex();
	gl_Position = VP* (gl_in[0].gl_Position + vec4(size/2,-size/2, size/2,0));
	EmitVertex();
	gl_Position = VP* (gl_in[0].gl_Position + vec4(size/2,size/2, size/2,0));
	EmitVertex();

	gl_Position = VP* (gl_in[0].gl_Position + vec4(-size/2,size/2, size/2,0));
	EmitVertex();

	gl_Position = VP* (gl_in[0].gl_Position + vec4(-size/2,size/2, size/2,0));
	EmitVertex();
	gl_Position = VP* (gl_in[0].gl_Position + vec4(-size/2,-size/2, size/2,0));
	EmitVertex();
	gl_Position = VP* (gl_in[0].gl_Position + vec4(-size/2,-size/2, -size/2,0));
	EmitVertex();
	gl_Position = VP* (gl_in[0].gl_Position + vec4(-size/2,size/2, -size/2,0));
	EmitVertex();
	gl_Position = VP* (gl_in[0].gl_Position + vec4(-size/2,size/2, size/2,0));
	EmitVertex();
	EndPrimitive();

	gl_Position = VP* (gl_in[0].gl_Position + vec4(-size/2,size/2, -size/2,0));
	EmitVertex();
	gl_Position = VP* (gl_in[0].gl_Position + vec4(size/2,size/2, -size/2,0));
	EmitVertex();
	EndPrimitive();

	gl_Position = VP* (gl_in[0].gl_Position + vec4(-size/2,-size/2, -size/2,0));
	EmitVertex();
	gl_Position = VP* (gl_in[0].gl_Position + vec4(size/2,-size/2, -size/2,0));
	EmitVertex();
	EndPrimitive();

	gl_Position = VP* (gl_in[0].gl_Position + vec4(-size/2,-size/2, size/2,0));
	EmitVertex();
	gl_Position = VP* (gl_in[0].gl_Position + vec4(size/2,-size/2, size/2,0));
	EmitVertex();
	EndPrimitive();
}
