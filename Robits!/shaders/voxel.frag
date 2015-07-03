#version 430

layout(binding = 0, r8ui)writeonly restrict uniform uimage3D dataTexture;

flat in int viewAxis;

uniform int voxelSize;

void main(){
	ivec3 temp = ivec3(gl_FragCoord.xy, gl_FragCoord.z * voxelSize);

	ivec3 texcoord = temp;
	if( viewAxis == 1 ){
		texcoord.x = temp.z;
		texcoord.z = temp.x;
	
	}
	else if( viewAxis == 2 ){
		texcoord.y = temp.z;
		texcoord.z = temp.y;
	}

	imageStore(dataTexture, abs(texcoord), uvec4(30));
}
