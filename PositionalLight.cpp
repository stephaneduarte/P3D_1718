#include "PositionalLight.h"



PositionalLight::PositionalLight()
{
	_position = new glm::vec3(0);
	_color = new glm::vec3(0,0,0);
}


PositionalLight::~PositionalLight()
{
	delete _color;
}

void PositionalLight::setPosition(float x, float y, float z) {
	_position->x = x;
	_position->y = y;
	_position->z = z;
	for (float i = 0; i < softShadows_N; i++) {
		for (float j = 0; j < softShadows_N; j++) {
			glm::vec3 * _position = new glm::vec3(0);
			_position->x = x + (i + rand() % 2) / softShadows_N;
			_position->y = y + (j + rand() % 2) / softShadows_N;
			_position->z = z;
			_positions.push_back(_position);
		}
	}
}

void PositionalLight::setColor(float x, float y, float z) {
	_color->x = x;
	_color->y = y;
	_color->z = z;
}

glm::vec3 * PositionalLight::getPosition(bool softShadows)
{
	if (!softShadows) return _position;
	int x = _position->x;
	int y = _position->y;
	int z = _position->z;
	glm::vec3 * position = new glm::vec3(0);
	position->x = x + rand();
	position->y = y + rand();
	position->z = z;
	return position;
	/*int position = int(rand() % _positions.size());
	return _positions.at(position);*/
}

glm::vec3 * PositionalLight::getColor()
{
	return _color;
}
