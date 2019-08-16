#ifndef __VIEWPOINT__
#define __VIEWPOINT__

#include "glm/vec3.hpp"
#include "glm/ext.hpp"

#include <iostream>
#include <math.h>

#define M_PI 3.141592653589793

class Viewpoint
{
private:
	glm::vec3 * _from;
	glm::vec3 * _at;
	glm::vec3 * _up;
	float _angle;
	float _hither; //near
	float _xres;
	float _yres;

	float _df;
	float _h;
	float _w;
	glm::vec3 _ze;
	glm::vec3 _ye;
	glm::vec3 _xe;

public:
	Viewpoint();
	~Viewpoint();

	//Setters
	void setFromVector(float x, float y, float z);
	void setAtVector(float x, float y, float z);
	void setUpVector(float x, float y, float z);
	void setAngle(float angle);
	void setHither(float hither);
	void setRes(float xres, float yres);

	//getters
	float getXRes();
	float getYRes();
	glm::vec3 * getFromVector();
	float getDf();
	float getH();
	float getW();
	glm::vec3 getXVector();
	glm::vec3 getYVector();
	glm::vec3 getZVector();

	//auxiliar functions
	void init();
};

#endif