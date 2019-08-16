#include "Cylinder.h"



Cylinder::Cylinder()
{
}


Cylinder::~Cylinder()
{
}

glm::vec3 Cylinder::getBaseCoordinates()
{
	return _base_coordinates;
}

glm::vec3 Cylinder::getApexCoordinates()
{
	return _apex_coordinates;
}

float Cylinder::getApexRadius()
{
	return _apex_radius;
}

float Cylinder::getBaseRadius()
{
	return _base_radius;
}

void Cylinder::setBaseCoordinates(float x, float y, float z)
{
	_base_coordinates = glm::vec3(x, y, z);
}

void Cylinder::setApexCoordinates(float x, float y, float z)
{
	_apex_coordinates = glm::vec3(x, y, z);
}

void Cylinder::setApexRadius(float apexRadius)
{
	_apex_radius = apexRadius;
}

void Cylinder::setBaseRadius(float baseRadius)
{
	_base_radius = baseRadius;
}

Material * Cylinder::getMaterial()
{
	return _object_material;
}

void Cylinder::setMaterial(Material * objectMaterial)
{
	_object_material = objectMaterial;
}
