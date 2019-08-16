#include "Scene.h"

Scene::Scene()
{
	_background_color = new glm::vec3(0, 0, 0);
}


Scene::~Scene()
{
}

bool Scene::load_nff(std::string filename) {
	try {
		std::ifstream nffFile(filename);
		if (nffFile) {
			std::string line;
			Material* _object_material = nullptr;
			while (std::getline(nffFile, line))
			{
				std::istringstream ss(line);
				std::istream_iterator<std::string> begin(ss), end;
				std::vector<std::string> values(begin, end);

				//BACKGROUND COLOR
				if (!values[0].compare("b")) {
					_background_color->x = ::atof(values[1].c_str());
					_background_color->y = ::atof(values[2].c_str());
					_background_color->z = ::atof(values[3].c_str());
				}

				//VIEWPOINT
				else if (!values[0].compare("v")) {
					_viewpoint = new Viewpoint();
					for (int i = 0; i < 6; i++) {
						std::getline(nffFile, line);
						std::istringstream ss(line);
						std::istream_iterator<std::string> begin(ss), end;
						std::vector<std::string> values(begin, end);
						if (i == 0 && !values[0].compare("from")) {
							_viewpoint->setFromVector(::atof(values[1].c_str()), ::atof(values[2].c_str()), ::atof(values[3].c_str()));
						}
						else if (i == 1 && !values[0].compare("at")) {
							_viewpoint->setAtVector(::atof(values[1].c_str()), ::atof(values[2].c_str()), ::atof(values[3].c_str()));
						}
						else if (i == 2 && !values[0].compare("up")) {
							_viewpoint->setUpVector(::atof(values[1].c_str()), ::atof(values[2].c_str()), ::atof(values[3].c_str()));
						}
						else if (i == 3 && !values[0].compare("angle")) {
							_viewpoint->setAngle(::atof(values[1].c_str()));
						}
						else if (i == 4 && !values[0].compare("hither")) {
							_viewpoint->setHither(::atof(values[1].c_str()));
						}
						else if (i == 5 && !values[0].compare("resolution")) {
							_viewpoint->setRes(::atof(values[1].c_str()), ::atof(values[2].c_str()));
						}
						else {
							std::cerr << "Bad viewpoint location format descriptor." << std::endl;
						}
					}
					_viewpoint->init();
				}

				//POSITIONAL LIGHTS
				else if (!values[0].compare("l")) {
					PositionalLight * _light = new PositionalLight();
					_light->setPosition(::atof(values[1].c_str()), ::atof(values[2].c_str()), ::atof(values[3].c_str()));
					_light->setColor(::atof(values[4].c_str()), ::atof(values[5].c_str()), ::atof(values[6].c_str()));
					_positional_lights.push_back(_light);
				}

				//OBJECT MATERIALS
				else if (!values[0].compare("f")) {
					_object_material = new Material();
					_object_material->setColor(::atof(values[1].c_str()), ::atof(values[2].c_str()), ::atof(values[3].c_str()));
					_object_material->setSpecs(::atof(values[4].c_str()), ::atof(values[5].c_str()), ::atof(values[6].c_str()));
					_object_material->setTransmittance(::atof(values[7].c_str()));
					_object_material->setIndexRefraction(::atof(values[8].c_str()));
					_object_materials.push_back(_object_material);
				}

				//SPHERES
				else if (!values[0].compare("s") && _object_material != nullptr) {
					Sphere* _sphere = new Sphere();
					_sphere->setMaterial(_object_material);
					_sphere->setCenterCoordinates(::atof(values[1].c_str()), ::atof(values[2].c_str()), ::atof(values[3].c_str()));
					_sphere->setRadius(::atof(values[4].c_str()));
					_sphere->generateBoundingBox();
					_objects.push_back(_sphere);
				}

				//TRIANGLES
				else if (!values[0].compare("p") && _object_material != nullptr) {
					Triangle * _triangle = new Triangle();
					_triangle->setMaterial(_object_material);
					for (int i = 0; i < ::atof(values[1].c_str()); i++) {
						std::getline(nffFile, line);
						std::istringstream ss(line);
						std::istream_iterator<std::string> begin(ss), end;
						std::vector<std::string> values(begin, end);
						_triangle->setNewPoint(glm::vec3(::atof(values[0].c_str()), ::atof(values[1].c_str()), ::atof(values[2].c_str())));
					}
					_triangle->generateBoundingBox();
					_objects.push_back(_triangle);
				}

				//PLAN
				else if (!values[0].compare("pl") && _object_material != nullptr) {
					//Falta tratar planos com BBs
					/*Plan* _plan = new Plan();
					_plan->setMaterial(_object_material);
					_plan->setNewPoint(glm::vec3(::atof(values[1].c_str()), ::atof(values[2].c_str()), ::atof(values[3].c_str())));
					_plan->setNewPoint(glm::vec3(::atof(values[4].c_str()), ::atof(values[5].c_str()), ::atof(values[6].c_str())));
					_plan->setNewPoint(glm::vec3(::atof(values[7].c_str()), ::atof(values[8].c_str()), ::atof(values[9].c_str())));
					_objects.push_back(_plan);*/
				}

				else {
					std::cerr << "Unknown descriptor:" << values[0] << std::endl;
				}
			}
			nffFile.close();
			_grid = new Grid(_objects);
		}
		else {
			std::cerr << "Couldn't open " << filename << "file." << std::endl;
			return false;
		}
		return true;
	}
	catch (int e) {
		return false;
	}
}

glm::vec3 * Scene::getBackgroundColor() {
	return _background_color;
}

Viewpoint * Scene::getViewpoint()
{
	return _viewpoint;
}

std::vector<Object*> Scene::getObjects()
{
	return _objects;
}

std::vector<PositionalLight*> Scene::getLights()
{
	return _positional_lights;
}

Grid * Scene::getGrid()
{
	return _grid;
}
