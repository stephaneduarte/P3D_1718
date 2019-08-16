#include "Cell.h"



Cell::Cell()
{
}


Cell::~Cell()
{
}

std::vector<Object*> Cell::getObjects()
{
	return _objects;
}

BoundingBox * Cell::getBoundingBox()
{
	return _bb;
}

void Cell::addObject(Object * object)
{
	_objects.push_back(object);
}
