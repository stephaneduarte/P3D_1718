#ifndef __TRIANGLE__
#define __TRIANGLE__

#include "Object.h"
#include <limits>

class Triangle : public Object
{

	private:
		std::vector<glm::vec3> _points;

	public:
		Triangle();
		virtual ~Triangle();

		void generateBoundingBox();
		bool rayIntersection(Ray * ray, glm::vec3 * intersectionPoint, glm::vec3 * normalIntersection, bool test);

		//getters
		std::vector<glm::vec3> getPoints();
		Material * getMaterial();

		//setters
		void setNewPoint(glm::vec3 point);
		void setMaterial(Material * object_material);
};

#endif