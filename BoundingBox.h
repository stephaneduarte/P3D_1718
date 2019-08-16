#ifndef __BOUNDINGBOX__
#define __BOUNDINGBOX__

#include "glm/vec3.hpp"
#include "Ray.h"

class BoundingBox
{
private:
	glm::vec3 _min;
	glm::vec3 _max;

public:
	BoundingBox();
	virtual ~BoundingBox();

	void setMinMax(glm::vec3 min, glm::vec3 max);

	glm::vec3 getMin();
	glm::vec3 getMax();

	bool isInside(glm::vec3 position);
	bool rayIntersection(Ray * ray, float &tprox, float &tdist, glm::vec3 &tmin, glm::vec3 &tmax);
	bool stepIntersection(float v0, float vd, float vmin, float vmax, float &tprox, float &tdist, float &tmin, float &tmax);
};

#endif
