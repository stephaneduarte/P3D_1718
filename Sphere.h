#ifndef __SPHERE__
#define __SPHERE__

#include "Object.h"

class Sphere : public Object
{
private:
	glm::vec3 _center_coordinates;
	float _radius;

public:
	Sphere();
	~Sphere();

	void generateBoundingBox();
	bool rayIntersection(Ray* ray, glm::vec3 * intersectionPoint, glm::vec3 * normalIntersection, bool test);


	//setters
	void setCenterCoordinates(float x, float y, float z);
	void setRadius(float radius);
	void setMaterial(Material * object_material);

	//getters
	glm::vec3 getCenterCoordinates();
	float getSphereRadius();
	Material * getMaterial();
};

#endif