#ifndef __OBJECT__
#define __OBJECT__

#include "glm/vec3.hpp"
#include "glm\glm.hpp"
#include "Material.h"
#include "BoundingBox.h"
#include <vector>
#include "Ray.h"

class Object
{
protected:
	Material* _object_material;
	BoundingBox * _bb = nullptr;

public:
	Object();
	~Object();

	virtual void generateBoundingBox();
	virtual bool rayIntersection(Ray* ray, glm::vec3 * intersectionPoint, glm::vec3 * normalIntersection, bool test);
	virtual Material* getMaterial();
	BoundingBox * getBoundingBox();
};

#endif