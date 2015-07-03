#pragma once

struct Material{
	Material():
		Ns(1.0),
		Ni(1.0),
		Tr(0.0),
		illum(2),
		map_Ka(0),
		map_Kd(0),
		map_d(0),
		map_bump(0)
	{
		Tf[0] = 1;
		Tf[1] = 1;
		Tf[2] = 1;

		Ka[0] = 1;
		Ka[1] = 1;
		Ka[2] = 1;

		Kd[0] = 1;
		Kd[1] = 1;
		Kd[2] = 1;

		Ks[0] = 1;
		Ks[1] = 1;
		Ks[2] = 1;
	}

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
	GLuint map_Ka;
	//Diffuse map
	GLuint map_Kd;
	//Alpha texture map
	GLuint map_d;
	//map_bump or bump
	GLuint map_bump;
};