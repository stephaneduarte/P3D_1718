#ifndef __SCENE__
#define __SCENE__

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <iterator>

#include "glm/vec3.hpp"
#include "Viewpoint.h"
#include "PositionalLight.h"
#include "Material.h"
#include "Plan.h"
#include "Sphere.h"
#include "Triangle.h"
#include "Grid.h"
#include "Object.h"

class Scene
{
private:
	glm::vec3 * _background_color;
	Viewpoint * _viewpoint;
	std::vector<PositionalLight*> _positional_lights;
	std::vector<Material*> _object_materials;
	std::vector<Object*> _objects;
	Grid * _grid = nullptr;

public:
	Scene();
	~Scene();

	//auxiliar functions
	bool load_nff(std::string filename);

	//getters
	glm::vec3 * getBackgroundColor();
	Viewpoint * getViewpoint();
	std::vector<Object*> getObjects();
	std::vector<PositionalLight*> getLights();
	Grid * getGrid();
};

#endif

