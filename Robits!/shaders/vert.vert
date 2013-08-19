#version 330
layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texture;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec3 tangent;
layout(location = 4) in vec3 bitangent;
out vec3 PositionWorldSpace;
out vec2 UV;

out vec3 LightDir;
out vec3 EyeDir;

out vec3 NormalCamSpace;

out vec4 ShadowCoord;
uniform mat4 MVP;
uniform mat4 M;
uniform mat4 V;
uniform vec3 lightPos;
uniform bool toggleTextures;

void main() {
	PositionWorldSpace = (M * vec4(position,1.0)).xyz;
	vec3 PositionCamSpace = (V*M*vec4(position, 1.0)).xyz;
	vec3 LightDirCamSpace = (V*vec4(lightPos, 1.0)).xyz - PositionCamSpace;
	gl_Position = MVP * vec4(position, 1.0 );

	ShadowCoord =vec4( PositionWorldSpace - lightPos,1);

	NormalCamSpace =(V*M * vec4(normal,0.0)).xyz;

	mat3 TBN = mat3 (1.0);
	if(toggleTextures){
		vec3 TangentCamSpace =(V*M* normalize(vec4(tangent, 0.0))).xyz;
		vec3 BitangentCamSpace = (V*M* normalize(vec4(bitangent, 0.0))).xyz;
		TBN = transpose(mat3(TangentCamSpace, BitangentCamSpace, NormalCamSpace));
	}
	LightDir = TBN * LightDirCamSpace;
	EyeDir = TBN * (-PositionCamSpace);

	UV = texture;
}

