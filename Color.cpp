#include "Color.h"



Color::Color()
{
}

Color::Color(float r, float g, float b) {
	_r = r;
	_g = g;
	_b = b;
}

Color::~Color()
{
}

float Color::getR() {
	return _r;
}

float Color::getG() {
	return _g;
}

float Color::getB() {
	return _b;
}

void Color::setColor(float r, float g, float b)
{
	_r = r;
	_g = g;
	_b = b;
}
