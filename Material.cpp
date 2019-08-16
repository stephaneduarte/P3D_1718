#include "Material.h"



Material::Material()
{
	_color = new glm::vec3();
	_specs = new glm::vec3();
}


Material::~Material()
{
	delete _color;
	delete _specs;
}

void Material::setColor(float r, float g, float b)
{
	_color->x = r;
	_color->y = g;
	_color->z = b;
}

void Material::setSpecs(float diffuse, float specular, float shine)
{
	_specs->x = diffuse;
	_specs->y = specular;
	_specs->z = shine;
}

void Material::setTransmittance(float transmittance)
{
	_transmittance = transmittance;
}

void Material::setIndexRefraction(float refraction)
{
	_refraction = refraction;
}

glm::vec3 * Material::getColor()
{
	return _color;
}

float Material::getDiffuse()
{
	return _specs->x;
}

float Material::getSpecular()
{
	return _specs->y;
}

float Material::getShininess()
{
	return _specs->z;
}

float Material::getTransmittance()
{
	return _transmittance;
}

float Material::getRefraction()
{
	return _refraction;
}
