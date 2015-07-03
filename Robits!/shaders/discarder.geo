#version 430

in vec3 PositionWorldSpace[];
in vec2 UV[];

in vec3 LightDir[];
in vec3 EyeDir[];

in vec3 NormalCamSpace[];

void main(){
}	vec3 normal = normalize(cross(PositionWorldSpace[1] - PositionWorldSpace[0], PositionWorldSpace[2] - PositionWorldSpace[0]));

	float NdotXAxis = abs( normal.x );
	float NdotYAxis = abs( normal.y );
	float NdotZAxis = abs( normal.z );
	
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

}
