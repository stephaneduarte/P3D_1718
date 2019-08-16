#ifndef __RAY__
#define __RAY__

#include <limits>

#include "glm/vec3.hpp"
#include "Color.h"

class Ray
{
private:
	glm::vec3 _origin;
	glm::vec3 _direction;
	float _mindist = FLT_MAX; //distance from the intersection point
	Color * _color = nullptr;

public:
	Ray();
	~Ray();

	//Setters
	void setOrigin(float x, float y, float z);
	void setDirection(float x, float y, float z);
	void setDirection(glm::vec3 direction);
	void setColor(Color * color);
	void setMinDist(float distance);

	//getters
	glm::vec3 getOrigin();
	glm::vec3 getDirection();
	float getMinDist();
};

#endif