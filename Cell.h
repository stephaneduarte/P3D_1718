#ifndef __CELL__
#define __CELL__

#include "Object.h"
#include <vector>

class Cell
{
private:
	std::vector<Object*> _objects;
	BoundingBox * _bb;

public:
	Cell();
	~Cell();

	std::vector<Object*> getObjects();
	BoundingBox * getBoundingBox();
	void addObject(Object * object);
};

#endif