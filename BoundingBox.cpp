#include "BoundingBox.h"


BoundingBox::BoundingBox() {
	_min = glm::vec3(0);
	_max = glm::vec3(0);
}

BoundingBox::~BoundingBox() {}

void BoundingBox::setMinMax(glm::vec3 min, glm::vec3 max) {
	_min = min;
	_max = max;
}

glm::vec3 BoundingBox::getMin() {
	return _min;
}

glm::vec3 BoundingBox::getMax() {
	return _max;
}

bool BoundingBox::isInside(glm::vec3 position) {
	if (position.x > _min.x && position.x < _max.x && position.y > _min.y && position.y < _max.y && position.z > _min.z && position.z < _max.z) return true;
	return false;
}

bool BoundingBox::rayIntersection(Ray * ray, float &tprox, float &tdist, glm::vec3 &tmin, glm::vec3 &tmax)
{
	tprox = std::numeric_limits<float>::min();
	tdist = std::numeric_limits<float>::max();

	glm::vec3 v0 = ray->getOrigin();
	glm::vec3 vd = ray->getDirection();

	bool xDirection = stepIntersection(v0.x, vd.x, _min.x, _max.x, tprox, tdist, tmin.x, tmax.x);
	bool yDirection = stepIntersection(v0.y, vd.y, _min.y, _max.y, tprox, tdist, tmin.y, tmax.y);
	bool zDirection = stepIntersection(v0.z, vd.z, _min.z, _max.z, tprox, tdist, tmin.z, tmax.z);

	if (xDirection && yDirection && zDirection) return true;
	return false;
}

bool BoundingBox::stepIntersection(float v0, float vd, float vmin, float vmax, float &tprox, float &tdist, float &tmin, float &tmax) {
	if (vd == 0) {
		if (v0 < vmin || v0 > vmax) return false;
		else return true;
	}

	tmin = (vmin - v0) / vd;
	tmax = (vmax - v0) / vd;
	if (tmin > tmax) {
		float aux = tmin;
		tmin = tmax;
		tmax = aux;
	}

	if (tmin > tprox) tprox = tmin;
	if (tmax < tdist) tdist = tmax;

	if (tprox > tdist) return false;
	if (tdist < 0) return false;

	return true;
}