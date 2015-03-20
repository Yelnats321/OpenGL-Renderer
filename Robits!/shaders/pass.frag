#version 330 core

in vec2 UV;
layout(location = 0) out vec3 color;

uniform sampler2D passTexture;
//27x48 +2pad
uniform sampler2D asciiTexture;

//should be integral values (they are floats for calculation purposes)
const float pixelX = 5, pixelY=8;
const float screenX = 800, screenY=600;

void main(){
	//color = vec3(UV,0);
	vec2 nUV;
	nUV.x =int(UV.x*screenX/pixelX)*pixelX/screenX;
	nUV.y =int(UV.y*screenY/pixelY)*pixelY/screenY;
	float bright =int((log(dot(texture(passTexture,nUV).rgb,vec3(1.f))/3.f)+4)/4*9);
	bright = clamp(bright,0,8);
	vec2 asciiUV;
	asciiUV.x = mod(int(UV.x*screenX), pixelX)/pixelX/9+bright/9;
	asciiUV.y = mod(int(UV.y*screenY), pixelY)/pixelY;
	//color = vec3(nUV,0);
	color = texture(asciiTexture,asciiUV).rgb*texture(passTexture,nUV).rgb;
	//MATRIX MODE
	color = texture(asciiTexture,asciiUV).rgb*dot(texture(passTexture,nUV).rgb,vec3(0.212,0.715,0.072))*vec3(0,204/255.f,0);
}	
