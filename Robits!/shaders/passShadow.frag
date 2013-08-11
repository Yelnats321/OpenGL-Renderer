#version 330 core
 
layout(location = 0) out float fragmentDepth;
//layout(origin_upper_left, pixel_center_integer) in vec4 gl_FragCoord;
 
//uniform sampler2D renderedTexture;
//uniform float time;

/*float rand(vec2 co){
	return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}*/
 
void main(){
	fragmentDepth = gl_FragCoord.z;
	//color = texture(renderedTexture, UV).rgb;
	//color = color+(mod(gl_FragCoord.x, 5)-2)*0.05 + (mod(gl_FragCoord.y,5)-2)*0.05;
	//color = (texture(renderedTexture, UV).xyz) + vec3(0.3)*rand(UV.xy+time);
}
