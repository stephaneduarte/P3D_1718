#ifndef __POSITIONAL_LIGHT__
#define __POSITIONAL_LIGHT__

#include "glm/vec3.hpp"
#include <vector>
#include <iostream>

#define softShadows_N 3

class PositionalLight
{
private:
	glm::vec3 * _position;
	std::vector<glm::vec3*> _positions;
	glm::vec3 * _color;

public:
	PositionalLight();
	~PositionalLight();

	//setters
	void setPosition(float x, float y, float z);
	void setColor(float x, float y, float z);

	//getters
	glm::vec3 * getPosition(bool softShadows);
	glm::vec3 * getColor();
};

#endif
