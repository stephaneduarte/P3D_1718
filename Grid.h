#ifndef __GRID__
#define __GRID__

#define mFACTOR 2 //Start with m=2

#include "Object.h"
#include "Cell.h"
#include <vector>
#include <iostream>
#include "glm/ext.hpp"

class Grid
{

private:
	BoundingBox * _bb = nullptr;
	std::vector<Cell*> _cells;
	glm::vec3 _N = glm::vec3(0);
	glm::vec3 _dim = glm::vec3(0);

public:
	Grid();
	Grid(std::vector<Object*> objects);
	~Grid();

	Object* rayIntersection(Ray* ray, glm::vec3 * intersectionPoint, glm::vec3 * normalIntersection);
	void calculateTnext(float &tnext, float &step, float &stop, float tmin, float cellPos, float dt, float rayDirection, int n);
	float getDistance(glm::vec3 firstpoint, glm::vec3 secondpoint);
	Object * traverseGrid(glm::vec3 cellPos, Ray * ray, glm::vec3 tnext, glm::vec3 dt, glm::vec3 step, glm::vec3 stop, glm::vec3 * intersectionPoint, glm::vec3 * normalIntersection);
};

#endif