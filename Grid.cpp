#include "Grid.h"



Grid::Grid()
{
}

Grid::Grid(std::vector<Object*> objects) {
	//Create BB
	_bb = new BoundingBox();
	glm::vec3 min = glm::vec3(std::numeric_limits<float>::max());
	glm::vec3 max = glm::vec3(std::numeric_limits<float>::min());
	//consoante a bounding box de todos os objs
	for (int i = 0; i < objects.size(); i++) {
		glm::vec3 object_min = objects[i]->getBoundingBox()->getMin();
		if (object_min.x < min.x) min.x = object_min.x - 0.0001f;
		if (object_min.y < min.y) min.y = object_min.y - 0.0001f;
		if (object_min.z < min.z) min.z = object_min.z - 0.0001f;
		glm::vec3 object_max = objects[i]->getBoundingBox()->getMax();
		if (object_max.x > max.x) max.x = object_max.x + 0.0001f;
		if (object_max.y > max.y) max.y = object_max.y + 0.0001f;
		if (object_max.z > max.z) max.z = object_max.z + 0.0001f;
	}
	_bb->setMinMax(min, max);

	//Dimensions of the grid
	_dim = max - min;

	//A scene with N objects will have O(N^1/3) cells in each direction
	float s = pow(_dim.x * _dim.y * _dim.z / objects.size(), 1.f/3.f);
	_N.x = int(mFACTOR * _dim.x / s) + 1;
	_N.y = int(mFACTOR * _dim.y / s) + 1;
	_N.z = int(mFACTOR * _dim.z / s) + 1;

	//Create cells
	for (int i = 0; i < (_N.x * _N.y * _N.z); i++) {
		Cell * cell = new Cell();
		_cells.push_back(cell);
	}

	//Cells and Objects
	//Coloca os objetos nas celas consoante o seu BB
	for (int i = 0; i < objects.size(); i++) {
		glm::vec3 objBBmin = objects[i]->getBoundingBox()->getMin();
		glm::vec3 objBBmax = objects[i]->getBoundingBox()->getMax();

		/* Compute indices of both cells that contain min and max coord of obj bbox */
		int ixmin = glm::clamp((int)((objBBmin.x - _bb->getMin().x) * _N.x / _dim.x), 0, int(_N.x - 1));
		int iymin = glm::clamp((int)((objBBmin.y - _bb->getMin().y) * _N.y / _dim.y), 0, int(_N.y - 1));
		int izmin = glm::clamp((int)((objBBmin.z - _bb->getMin().z) * _N.z / _dim.z), 0, int(_N.z - 1));
		int ixmax = glm::clamp((int)((objBBmax.x - _bb->getMin().x) * _N.x / _dim.x), 0, int(_N.x - 1));
		int iymax = glm::clamp((int)((objBBmax.y - _bb->getMin().y) * _N.y / _dim.y), 0, int(_N.y - 1));
		int izmax = glm::clamp((int)((objBBmax.z - _bb->getMin().z) * _N.z / _dim.z), 0, int(_N.z - 1));

		/* insert obj to the overlaped cells */
		for (int iz = izmin; iz <= izmax; iz++) {
			for (int iy = iymin; iy <= iymax; iy++) {
				for (int ix = ixmin; ix <= ixmax; ix++) {
					_cells[ix + _N.x * iy + _N.x * _N.y * iz]->addObject(objects[i]);
				}
			}
		}
	}
}


Grid::~Grid()
{
}

Object* Grid::rayIntersection(Ray* ray, glm::vec3 * intersectionPoint, glm::vec3 * normalIntersection) {

	float tprox;
	float tdist;
	glm::vec3 tmin;
	glm::vec3 tmax;
	/*if the ray misses the Grid’s BB*/
	if (!_bb->rayIntersection(ray, tprox, tdist, tmin, tmax)) return false;

	glm::vec3 cellPos;
	glm::vec3 rayOrigin = ray->getOrigin();
	/* Calculate Starting Cell */
	if (_bb->isInside(rayOrigin)) {
		/*if the ray starts inside the Grid find the cell that contain the ray origin*/
		cellPos.x = glm::clamp((int)((rayOrigin.x - _bb->getMin().x) * _N.x / _dim.x), 0, int(_N.x - 1));
		cellPos.y = glm::clamp((int)((rayOrigin.y - _bb->getMin().y) * _N.y / _dim.y), 0, int(_N.y - 1));
		cellPos.z = glm::clamp((int)((rayOrigin.z - _bb->getMin().z) * _N.z / _dim.z), 0, int(_N.z - 1));
	}
	else {
		/*find the cell where the ray hits the Grid from the outside*/
		glm::vec3 hitPoint = rayOrigin + tprox * ray->getDirection();
		cellPos.x = glm::clamp((int)((hitPoint.x - _bb->getMin().x) * _N.x / _dim.x), 0, int(_N.x - 1));
		cellPos.y = glm::clamp((int)((hitPoint.y - _bb->getMin().y) * _N.y / _dim.y), 0, int(_N.y - 1));
		cellPos.z = glm::clamp((int)((hitPoint.z - _bb->getMin().z) * _N.z / _dim.z), 0, int(_N.z - 1));
	}

	/*Set up Grid Traversal
	Stepping the ray through the Grid*/
	glm::vec3 dt = glm::vec3((tmax.x - tmin.x) / _N.x, (tmax.y - tmin.y) / _N.y, (tmax.z - tmin.z) / _N.z);

	/*initial cell*/
	glm::vec3 tnext;
	glm::vec3 step;
	glm::vec3 stop;
	calculateTnext(tnext.x, step.x, stop.x, tmin.x, cellPos.x, dt.x, ray->getDirection().x, _N.x); //direction xx'
	calculateTnext(tnext.y, step.y, stop.y, tmin.y, cellPos.y, dt.y, ray->getDirection().y, _N.y); //direction yy'
	calculateTnext(tnext.z, step.z, stop.z, tmin.z, cellPos.z, dt.z, ray->getDirection().z, _N.z); //direction zz'
	
	return traverseGrid(cellPos, ray, tnext, dt, step, stop, intersectionPoint, normalIntersection);
}

void Grid::calculateTnext(float &tnext, float &step, float &stop, float tmin, float cellPos, float dt, float rD, int n) {
	if (rD > 0) {
		tnext = tmin + (cellPos + 1) * dt;
		step = 1;
		stop = n;
	}
	else {
		tnext = tmin + (n - cellPos) * dt;
		step = -1;
		stop = -1;
	}
	if (rD == 0) {
		tnext = std::numeric_limits<float>::max(); //why???
		step = -1; //just to initialize, never used
		stop = -1;
	}
}

Object * Grid::traverseGrid(glm::vec3 cellPos, Ray * ray, glm::vec3 tnext, glm::vec3 dt, glm::vec3 step, glm::vec3 stop, glm::vec3 * intersectionPoint, glm::vec3 * normalIntersection) {
	while (true) {

		std::vector<Object*> objects = _cells[cellPos.x + _N.x * cellPos.y + _N.x * _N.y * cellPos.z]->getObjects();

		Object* hitObject = nullptr;
		float min = std::numeric_limits<float>::max();

		glm::vec3 * auxIntersectionPoint = new glm::vec3();
		glm::vec3 * auxNormalIntersection = new glm::vec3();
		for (int i = 0; i < objects.size(); i++) {
			if (objects[i]->rayIntersection(ray, auxIntersectionPoint, auxNormalIntersection, false)) {
				float auxDist = getDistance(ray->getOrigin(), *auxIntersectionPoint);
				if (auxDist < min) {
					hitObject = objects[i];
					min = auxDist;
					*intersectionPoint = *auxIntersectionPoint;
					*normalIntersection = *auxNormalIntersection;
				}
			}
		}
		delete(auxIntersectionPoint);
		delete(auxNormalIntersection);

		if (tnext.x < tnext.y && tnext.x < tnext.z) {
			if (hitObject && min < tnext.x) return hitObject;
			tnext.x += dt.x;
			cellPos.x += step.x;
			if (cellPos.x == stop.x) return nullptr;
		}
		else if (tnext.y < tnext.z) {
			if (hitObject && min < tnext.y) return hitObject;
			tnext.y += dt.y;
			cellPos.y += step.y;
			if (cellPos.y == stop.y) return nullptr;
		}
		else {
			if (hitObject && min < tnext.z) return hitObject;
			tnext.z += dt.z;
			cellPos.z += step.z;
			if (cellPos.z == stop.z) return nullptr;
		}
	}
	return nullptr;
}

float Grid::getDistance(glm::vec3 firstpoint, glm::vec3 secondpoint) {
	return abs(pow((pow(secondpoint.x-firstpoint.x,2) + pow(secondpoint.y-firstpoint.y,2) + pow(secondpoint.z-firstpoint.z,2)), 1.f / 2.f));
}