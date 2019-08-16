#include "Sphere.h"



Sphere::Sphere()
{
}


Sphere::~Sphere()
{
}

void Sphere::setCenterCoordinates(float x, float y, float z)
{
	_center_coordinates = glm::vec3(x,y,z);

}

void Sphere::setRadius(float radius)
{
	_radius = radius;
}

void Sphere::setMaterial(Material * object_material)
{
	_object_material = object_material;
}

glm::vec3 Sphere::getCenterCoordinates()
{
	return _center_coordinates;
}

float Sphere::getSphereRadius()
{
	return _radius;
}

Material * Sphere::getMaterial()
{
	return _object_material;
}

void Sphere::generateBoundingBox() {
	_bb = new BoundingBox();
	_bb->setMinMax(_center_coordinates - _radius, _center_coordinates + _radius);
}

bool Sphere::rayIntersection(Ray * ray, glm::vec3 * intersectionPoint, glm::vec3 * normalIntersection, bool test)
{
	//get ray info
	glm::vec3 rayOrigin = ray->getOrigin();
	glm::vec3 rayDirection = ray->getDirection();

	//normalize direction (not sure if needed)
	rayDirection = glm::normalize(rayDirection);

	//get sphere info
	glm::vec3 centerSphere = this->getCenterCoordinates();
	float sphereRadius = this->getSphereRadius();

	//calculate square distance
	float distance = pow((centerSphere.x - rayOrigin.x), 2) +
		pow((centerSphere.y - rayOrigin.y), 2) +
		pow((centerSphere.z - rayOrigin.z), 2);

	//calculate b
	float b = rayDirection.x * (centerSphere.x - rayOrigin.x) +
		rayDirection.y * (centerSphere.y - rayOrigin.y) +
		rayDirection.z * (centerSphere.z - rayOrigin.z);

	//condition
	if (distance > pow(sphereRadius, 2) && b < 0) return false;

	//calculate R
	float r = pow(b, 2) - distance + pow(sphereRadius, 2);

	//test
	if (r < 0) return false;
	//evita calculos desnecessarios
	if (test) return true;

	float ti;
	if (distance > pow(sphereRadius, 2)) ti = b - sqrt(r);
	else ti = b + sqrt(r);

	*intersectionPoint = rayOrigin + rayDirection * ti;

	float aux = 1;
	if (distance < pow(sphereRadius, 2)) aux = -1;

	*normalIntersection = glm::normalize(((*intersectionPoint - centerSphere) / sphereRadius) * aux);

	return true;
}
