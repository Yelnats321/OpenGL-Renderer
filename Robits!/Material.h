#pragma once
#include <string>

using std::string;

struct Material{
	string name;
	//Specular exponent
	float Ns;
	//Optical density, index of refraction
	float Ni;
	//d or Tr, Transparency
	float Tr;
	//Transmision filter, aka what colors will be filtered out(0) or left (1)
	float Tf[3];
	//Illumination type:
	/*0. Color on and Ambient off
	1. Color on and Ambient on
	2. Highlight on
	3. Reflection on and Ray trace on
	4. Transparency: Glass on, Reflection: Ray trace on
	5. Reflection: Fresnel on and Ray trace on
	6. Transparency: Refraction on, Reflection: Fresnel off and Ray trace on
	7. Transparency: Refraction on, Reflection: Fresnel on and Ray trace on
	8. Reflection on and Ray trace off
	9. Transparency: Glass on, Reflection: Ray trace off
	10. Casts shadows onto invisible surfaces*/
	int illum;
	//Ambient color multiply
	float Ka[3];
	//Diffuse color multiply
	float Kd[3];
	//Specular color mutiply
	float Ks[3];
	//Ambient map
	string map_Ka;
	//Diffuse map
	string map_Kd;
	//Alpha texture map
	string map_d;
	//map_bump or bump
	string map_bump;
};