#ifndef __PLAN__
#define __PLAN__

#include "glm/vec3.hpp"
#include "glm/glm.hpp"
#include "glm/ext.hpp"
#include "Object.h"

class Plan : public Object
{
private:
	std::vector<glm::vec3> _points; //3 points to define the plan

public:
	Plan();
	~Plan();

	bool rayIntersection(Ray * ray, glm::vec3 * intersectionPoint, glm::vec3 * normalIntersection, bool test);

	//getters
	std::vector<glm::vec3> getPoints();
	Material* Plan::getMaterial();

	//setters
	void setNewPoint(glm::vec3 point);
	void setMaterial(Material * object_material);

	//auxiliar functions
	glm::vec3 calculateNormal();
};

#endif