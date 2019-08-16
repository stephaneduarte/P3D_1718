#include "Object.h"

Object::Object()
{
}


Object::~Object()
{
}


void Object::generateBoundingBox() {
	_bb = new BoundingBox();
}

bool Object::rayIntersection(Ray * ray, glm::vec3 * intersectionPoint, glm::vec3 * normalIntersection, bool test)
{
	return false;
}

Material * Object::getMaterial()
{
	return _object_material;
}

BoundingBox * Object::getBoundingBox() {
	return _bb;
}