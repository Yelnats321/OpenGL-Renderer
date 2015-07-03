#version 430

layout(triangles) in;
layout(triangle_strip, max_vertices=3) out;

flat out int viewAxis;

uniform mat4 lookX;
uniform mat4 lookY;
uniform mat4 lookZ;


void main(){
	vec3 normal = normalize(cross(gl_in[1].gl_Position.xyz - gl_in[0].gl_Position.xyz, gl_in[2].gl_Position.xyz - gl_in[0].gl_Position.xyz));

	float NdotXAxis = abs( normal.x );
	float NdotYAxis = abs( normal.y );
	float NdotZAxis = abs( normal.z );
	
	mat4 view;
	if(NdotXAxis > NdotYAxis && NdotXAxis>NdotZAxis){
		view = lookX;
		viewAxis = 1;
	}
	else if(NdotYAxis > NdotXAxis && NdotYAxis>NdotZAxis){
		view = lookY;
		viewAxis = 2;
	}
	else{
		view = lookZ;
		viewAxis = 3;
	}
	
	for(int i = 0; i< 3; i++){
		gl_Position = view* gl_in[i].gl_Position;
		EmitVertex();
	}
	EndPrimitive();
}
