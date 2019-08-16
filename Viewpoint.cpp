#include "Viewpoint.h"

Viewpoint::Viewpoint()
{
	_from = new glm::vec3();
	_up = new glm::vec3();
	_at = new glm::vec3();
}


Viewpoint::~Viewpoint()
{
	delete _from;
	delete _up;
	delete _at;
}

void Viewpoint::setFromVector(float x, float y, float z){
	_from->x = x;
	_from->y = y;
	_from->z = z;
}

void Viewpoint::setAtVector(float x, float y, float z) {
	_at->x = x;
	_at->y = y;
	_at->z = z;
}

void Viewpoint::setUpVector(float x, float y, float z) {
	_up->x = x;
	_up->y = y;
	_up->z = z;
}

void Viewpoint::setAngle(float angle) {
	_angle = angle;
}

void Viewpoint::setHither(float hither) {
	_hither = hither;
}

void Viewpoint::setRes(float xres, float yres) {
	_xres = xres;
	_yres = yres;
}

float Viewpoint::getXRes()
{
	return _xres;
}

float Viewpoint::getYRes()
{
	return _yres;
}

glm::vec3 * Viewpoint::getFromVector()
{
	return _from;
}

float Viewpoint::getDf()
{
	return _df;
}

float Viewpoint::getH()
{
	return _h;
}

float Viewpoint::getW()
{
	return _w;
}

glm::vec3 Viewpoint::getXVector()
{
	return _xe;
}

glm::vec3 Viewpoint::getYVector()
{
	return _ye;
}

glm::vec3 Viewpoint::getZVector()
{
	return _ze;
}

void Viewpoint::init()
{
	_df = glm::length(*_from - *_at);
	_h = 2 * _df * tan(0.5 * (_angle * M_PI / 180) ); //angulo originar em graus
	_w = _xres / _yres * _h;

	//Tirei multiplicacoes dos slides faz-se norma
	_ze = glm::normalize(*_from - *_at);
	_xe = glm::normalize(glm::cross(*_up, _ze));
	_ye = glm::cross(_ze, _xe);
}
