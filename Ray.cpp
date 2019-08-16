#include "Ray.h"



Ray::Ray()
{
}


Ray::~Ray()
{
	delete _color;
}

void Ray::setOrigin(float x, float y, float z)
{
	_origin = glm::vec3(x, y, z);
}

void Ray::setDirection(float x, float y, float z) {
	_direction = glm::vec3(x, y, z);
}

void Ray::setDirection(glm::vec3 direction)
{
	_direction = direction;
}

void Ray::setColor(Color * color)
{
	_color = color;
}

void Ray::setMinDist(float distance)
{
	_mindist = distance;
}

glm::vec3 Ray::getOrigin()
{
	return _origin;
}

glm::vec3 Ray::getDirection()
{
	return _direction;
}

float Ray::getMinDist()
{
	return _mindist;
}
