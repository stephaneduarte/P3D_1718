#ifndef __MATERIAL__
#define __MATERIAL__

#include "glm/vec3.hpp"

class Material
{
private:
	glm::vec3 * _color;
	glm::vec3 * _specs; //diffuse, specular, shininess
	float _transmittance;
	float _refraction;

public:
	Material();
	~Material();

	//setters
	void setColor(float r, float g, float b);
	void setSpecs(float diffuse, float specular, float shine);
	void setTransmittance(float transmittance);
	void setIndexRefraction(float refraction);

	//getters
	glm::vec3 * getColor();
	float getDiffuse();
	float getSpecular();
	float getShininess();
	float getTransmittance();
	float getRefraction();
};

#endif