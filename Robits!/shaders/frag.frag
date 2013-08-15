#version 330

in vec3 PositionWorldSpace;
in vec2 UV;

in vec3 LightDir;
in vec3 EyeDir;
in vec3 NormalCamSpace;

in vec4 ShadowCoord;

uniform vec3 lightPos;
uniform float ambientIntensity;
uniform sampler2D ambTex;
uniform sampler2D difTex;
//uniform sampler2D specTex;
uniform sampler2D alphaMask;
uniform sampler2D normalMap;
uniform samplerCubeShadow shadowMap;
uniform vec3 ambientColor;
uniform vec3 specularColor;
uniform vec3 diffuseColor;
uniform float shineFactor;

layout(location = 0)out vec3 outColor;

float VectorToDepthValue(vec3 Vec){
    vec3 AbsVec = abs(Vec);
    float LocalZcomp = max(AbsVec.x, max(AbsVec.y, AbsVec.z));

    const float f = 50.0;
    const float n = 0.1;
    float NormZComp = (f+n) / (f-n) - (2*f*n)/(f-n)/LocalZcomp;
    return (NormZComp + 1.0) * 0.5;
}


void main(){
	vec2 uv = vec2(UV.x, 1-UV.y);
	if(texture(alphaMask, uv).r != 1){
		discard;
	}

	vec3 n = normalize(texture2D(normalMap, uv ).rgb*2-1);

	vec3 l = normalize(LightDir);
	float cosTheta = clamp(dot(n, l),0,1);

	vec3 E = normalize(EyeDir);

	vec3 halfAng = normalize (l + E);

	/*float gauss = acos(dot(n,halfAng));
	float expo = gauss/0.01;
	expo= -(expo * expo);
	float cosAlpha = exp(expo);*/

	float cosAlpha = clamp(dot(n, halfAng),0,1);

	cosAlpha = pow(cosAlpha, shineFactor);
	cosAlpha = cosTheta==0?0:cosAlpha;

	float lightPower = 3;
	vec3 lightVec = lightPos-PositionWorldSpace;
	//float attenIntensity = 1;
	float attenIntensity = 1/(1.0 +dot(lightVec, lightVec));

	//float bias = 0.005*tan(acos(cosTheta)); // cosTheta is dot( n,l ), clamped between 0 and 1
	//bias = clamp(bias, 0,0.01);
	float bias = 0.0025;
	/*float visibility = 1;
	float texDist = texture(shadowMap, ShadowCoord.xyz/ShadowCoord.w).x;
	float dist = VectorToDepthValue(lightVec);
	if(texDist < dist-bias)
		visibility = 0.1;*/
	float visibility = texture(shadowMap, vec4(ShadowCoord.xyz/ShadowCoord.w, VectorToDepthValue(lightVec)-bias));
	outColor =texture(difTex, uv).rgb* diffuseColor * lightPower* cosTheta *attenIntensity * visibility+
		/*texture(specTex, uv).rgb**/ specularColor* lightPower* cosAlpha *attenIntensity * visibility+
		texture(ambTex, uv).rgb* ambientColor* ambientIntensity;
}
