#include "Triangle.h"

Triangle::Triangle() {}


Triangle::~Triangle() {}

std::vector<glm::vec3> Triangle::getPoints()
{
	return _points;
}

void Triangle::setNewPoint(glm::vec3 point) {

	_points.push_back(point);
}

void Triangle::setMaterial(Material * object_material){

	_object_material = object_material;
}

Material * Triangle::getMaterial()
{
	return _object_material;
}

void Triangle::generateBoundingBox() {
	_bb = new BoundingBox();

	glm::vec3 min = glm::vec3(std::numeric_limits<float>::max());
	glm::vec3 max = glm::vec3(std::numeric_limits<float>::min());

	for (int i = 0; i < _points.size(); i++) {
		if (_points[i].x < min.x) min.x = _points[i].x;
		if (_points[i].x > max.x) max.x = _points[i].x;
		if (_points[i].y < min.y) min.y = _points[i].y;
		if (_points[i].y > max.y) max.y = _points[i].y;
		if (_points[i].z < min.z) min.z = _points[i].z;
		if (_points[i].z > max.z) max.z = _points[i].z;
	}

	_bb->setMinMax(min, max);
}

//Moller-Trumbore algorithm
bool Triangle::rayIntersection(Ray * ray, glm::vec3 * intersectionPoint, glm::vec3 * normalIntersection, bool test) {
	//get ray info
	glm::vec3 rayOrigin = ray->getOrigin();
	glm::vec3 rayDirection = ray->getDirection();

	//triangle points
	glm::vec3  vertex1 = this->getPoints()[0];
	glm::vec3  vertex2 = this->getPoints()[1];
	glm::vec3  vertex3 = this->getPoints()[2];

	//triangle normal
	glm::vec3 edge1 = vertex2 - vertex1;
	glm::vec3 edge2 = vertex3 - vertex1;
	glm::vec3 normal = glm::normalize(glm::cross(edge1, edge2));

	glm::vec3 h = glm::cross(rayDirection, edge2);
	float a = glm::dot(edge1, h);
	if (a > -0.0001f && a < 0.0001f) return false;

	float f = 1 / a;
	glm::vec3 s = rayOrigin - vertex1;
	float u = f * (glm::dot(s, h));
	if (u < 0.0f || u > 1.0f) return false;

	glm::vec3 q = glm::cross(s, edge1);
	float v = f * glm::dot(rayDirection, q);
	if (v < 0.0f || u + v > 1.0f) return false;

	float t = f * glm::dot(edge2, q);

	if (t <= 0.0001f) return false;
	if (test) return true;

	*intersectionPoint = rayOrigin + rayDirection * t;
	*normalIntersection = normal;

	return true;
}