#include "Plan.h"


Plan::Plan()
{
}


Plan::~Plan()
{
}

std::vector<glm::vec3> Plan::getPoints()
{
	return _points;
}

void Plan::setNewPoint(glm::vec3 point)
{
	_points.push_back(point);
}

void Plan::setMaterial(Material * object_material)
{
	_object_material = object_material;
}

Material* Plan::getMaterial()
{
	return _object_material;
}

glm::vec3 Plan::calculateNormal()
{
	return glm::normalize(glm::cross(_points[1] - _points[0], _points[2] - _points[0]));
}

bool Plan::rayIntersection(Ray * ray, glm::vec3 * intersectionPoint, glm::vec3 * normalIntersection, bool test) {
	//get ray info
	glm::vec3 rayOrigin = ray->getOrigin();
	glm::vec3 rayDirection = ray->getDirection();

	//get plan normal
	glm::vec3 planNormal = this->calculateNormal();

	//testa paralelismo
	float aux = glm::dot(planNormal, rayDirection);
	if (abs(aux) < 0.0001f) return false;

	float d = -glm::dot(this->getPoints()[0], planNormal);
	float ti = -(glm::dot(rayOrigin, planNormal) + d) / aux;

	if (ti <= 0.0f) return false;
	if (test) return true;

	*intersectionPoint = rayOrigin + rayDirection * ti;
	*normalIntersection = planNormal;

	return true;
}