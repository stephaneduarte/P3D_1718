 ///////////////////////////////////////////////////////////////////////
//
// P3D Course
// (c) 2016 by João Madeiras Pereira
// TEMPLATE: Whitted Ray Tracing NFF scenes and drawing points with Modern OpenGL
//
//You should develop your rayTracing( Ray ray, int depth, float RefrIndex) which returns a color and
// to develop your load_NFF function
//
///////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <string>
#include <stdio.h>
#include <math.h>
#include <cstdio>
#include <ctime>

#include <glew.h>
#include <freeglut.h>

#include "Scene.h"
#include "Ray.h"
#include "Color.h"
#include "Plan.h"
#include "BoundingBox.h"

#define CAPTION "ray tracer"

#define VERTEX_COORD_ATTRIB 0
#define COLOR_ATTRIB 1

#define MAX_DEPTH 4

//Antialiasing
bool ANTIALIASING = false;
#define ANTIALIASING_N 3

//softShadows
bool softShadows = false;

//Depth of Field
bool depthOfField = false;
int dof_N = 3;
float aperture = 15.0f;
float focalDistance = 3.5f; //distancia plano foco

//grid
bool withGrid = false;

// Points defined by 2 attributes: positions which are stored in vertices array and colors which are stored in colors array
float *colors;
float *vertices;

int size_vertices;
int size_colors;

GLfloat m[16];  //projection matrix initialized by ortho function

GLuint VaoId;
GLuint VboId[2];

GLuint VertexShaderId, FragmentShaderId, ProgramId;
GLint UniformId;

Scene* scene = NULL;
int RES_X, RES_Y;

/* Draw Mode: 0 - point by point; 1 - line by line; 2 - full frame */
int draw_mode=1; 

int WindowHandle = 0;

//HELPER TO DEBUG
bool PIXELTEST = false;
std::clock_t start;
double duration;


///////////////////////////////////////////////////////////////////////  RAY-TRACE SCENE

Color * rayTracing(Ray * ray, int depth, bool inside); //c++ problems

Ray * calculatePrimaryRay(float x, float y) {
	Ray * ray = new Ray();
	ray->setOrigin(scene->getViewpoint()->getFromVector()->x, scene->getViewpoint()->getFromVector()->y, scene->getViewpoint()->getFromVector()->z);
	//funcoes dos slides
	glm::vec3 xdirection = scene->getViewpoint()->getXVector() * scene->getViewpoint()->getW() * (x / RES_X - 0.5);
	glm::vec3 ydirection = scene->getViewpoint()->getYVector() * scene->getViewpoint()->getH() * (y / RES_Y - 0.5);
	glm::vec3 zdirection = scene->getViewpoint()->getZVector() * -scene->getViewpoint()->getDf();
	glm::vec3 direction = xdirection + ydirection + zdirection;
	direction = glm::normalize(direction);

	ray->setDirection(direction);

	return ray;
}

bool shadowRayTracing(Ray * ray) {
	if (withGrid) {
		glm::vec3 intersectionPoint;
		glm::vec3 normalIntersection;
		Object * auxObject = scene->getGrid()->rayIntersection(ray, &intersectionPoint, &normalIntersection);
		if (auxObject) return true;
		return false;
	}
	else {
		for (int i = 0; i < scene->getObjects().size(); i++) {
			if (scene->getObjects()[i]->rayIntersection(ray, nullptr, nullptr, true)) return true;
		}
		return false;
	}
}

Color * calculateBlinnPhongColor(Material * material, Ray * ray, glm::vec3 * intersectionPoint, glm::vec3 * normalIntersection, int depth, bool inside) {

	//vetor auxiliar para cor (r,g,b)
	glm::vec3 vColor = glm::vec3(0,0,0);

	//cor do material e specs do material
	glm::vec3 * matColor = material->getColor();
	float kd = material->getDiffuse();
	float ks = material->getSpecular();
	float alpha = material->getShininess();

	//para cada luz (multiple lightning)
	for (int i = 0; i < scene->getLights().size(); i++) {
		glm::vec3 position_light = *scene->getLights()[i]->getPosition(softShadows);

		//vetor luz (luz - intersecao)
		glm::vec3 lvector = glm::normalize(glm::vec3(position_light - *intersectionPoint));

		//testa sombra
		Ray * shadowRay = new Ray;
		//resolve problema de self-intersection
		glm::vec3 shadowOrigin = *intersectionPoint + lvector * 0.001;
		shadowRay->setOrigin(shadowOrigin.x, shadowOrigin.y, shadowOrigin.z);
		//raio em direcao a fonte de luz
		shadowRay->setDirection(lvector);
		//verifica se ha objetos entre o ponto e a fonte de luz
		bool isShadow = shadowRayTracing(shadowRay);
		delete shadowRay;

		//max(l*n, 0)
		float diffuse = glm::dot(lvector, *normalIntersection);
		if (diffuse > 0 && !isShadow) {
			//cor da luz
			glm::vec3 * lightColor = scene->getLights()[i]->getColor();
			//Id = kd * Ld * (l*n)
			vColor = vColor + (*lightColor) * diffuse * kd * (*matColor);

			//local specular reflection component
			//Is = ks * Ls * max((r*v)^alpha, 0)
			glm::vec3 refVector = glm::reflect(lvector, *normalIntersection);
			float dot = glm::dot(refVector, ray->getDirection());
			if (dot > 0) {
				float specular = pow(dot, alpha);
				vColor = vColor + (*lightColor) * specular * ks;
			}
		}
	}

	//Se ks > 0 - ha reflexao!
	if (ks > 0) {
		//direcao a refletir
		glm::vec3 reflectRayDirection = glm::reflect(ray->getDirection(), *normalIntersection);
		glm::vec3 reflectRayOrigin = *intersectionPoint + reflectRayDirection * 0.001f;
		Ray * reflectRay = new Ray();
		reflectRay->setOrigin(reflectRayOrigin.x, reflectRayOrigin.y, reflectRayOrigin.z);
		reflectRay->setDirection(reflectRayDirection);
		//verifica cor das reflexoes
		Color * reflectColor = rayTracing(reflectRay, depth + 1, inside);
		//soma cores
		vColor = vColor + glm::vec3(reflectColor->getR(), reflectColor->getG(), reflectColor->getB()) * ks;
		delete reflectRay;
		delete reflectColor;
	}

	//Se T > 0, o material tem transparencia
	float T = material->getTransmittance();
	if (T > 0) {

		//indices de refracao (consoante se esta dentro ou fora do objeto de intersecao)
		float eta;
		if (inside) eta = material->getRefraction();
		else eta = 1.0f / material->getRefraction();

		float I_dot_N = glm::dot(-ray->getDirection(), *normalIntersection);
		float k = 1 - (pow(eta, 2) * (1 - pow(I_dot_N,2)));

		if (k >= 0) {
			//direcao do raio
			float tPerpendicularComponent = sqrt(k);
			glm::vec3 refractDir = glm::normalize(eta*-(-ray->getDirection()) + (eta*I_dot_N - tPerpendicularComponent)*(*normalIntersection));

			Ray* refractedRay = new Ray();
			glm::vec3 origin = *intersectionPoint + 0.001f*refractDir;
			refractedRay->setOrigin(origin.x, origin.y, origin.z);
			refractedRay->setDirection(refractDir);

			Color * refractColor = rayTracing(refractedRay, depth + 1, !inside);
			vColor = vColor + glm::vec3(refractColor->getR(), refractColor->getG(), refractColor->getB()) * T;

			delete refractedRay;
			delete refractColor;
		}
	}

	return new Color(vColor.x, vColor.y, vColor.z);
}

Color * rayTracing(Ray * ray, int depth, bool inside){
	//Maxima profundidade de raios
	if (depth > MAX_DEPTH) {
		return new Color(scene->getBackgroundColor()->x, scene->getBackgroundColor()->y, scene->getBackgroundColor()->z);
	}

	glm::vec3 intersectionPoint;
	glm::vec3 normalIntersection;
	if (withGrid) {
		Object * object = scene->getGrid()->rayIntersection(ray, &intersectionPoint, &normalIntersection);
		if (object) {
			Material* materialIntersection = object->getMaterial();
			return calculateBlinnPhongColor(materialIntersection, ray, &intersectionPoint, &normalIntersection, depth, inside);
		}
		return new Color(scene->getBackgroundColor()->x, scene->getBackgroundColor()->y, scene->getBackgroundColor()->z);
	}
	else {
		//auxiliares
		bool intersection = false;
		glm::vec3 * auxIntersectionPoint = new glm::vec3();
		glm::vec3 * auxNormalIntersection = new glm::vec3();
		glm::vec3 intersectionPoint;
		glm::vec3 normalIntersection;
		Material* materialIntersection;
		Color * color;
		//verifica intersecao com esferas
		for (int i = 0; i < scene->getObjects().size(); i++) {
			if (scene->getObjects()[i]->rayIntersection(ray, auxIntersectionPoint, auxNormalIntersection, false)) {
				float distance = glm::length(glm::vec3(*auxIntersectionPoint - ray->getOrigin()));
				if (distance < ray->getMinDist()) {
					ray->setMinDist(distance);
					intersectionPoint = *auxIntersectionPoint;
					normalIntersection = *auxNormalIntersection;
					materialIntersection = scene->getObjects()[i]->getMaterial();
					intersection = true;
				}
			}
		}
		if (intersection) color = calculateBlinnPhongColor(materialIntersection, ray, &intersectionPoint, &normalIntersection, depth, inside);
		else color = new Color(scene->getBackgroundColor()->x, scene->getBackgroundColor()->y, scene->getBackgroundColor()->z);
		//delete
		delete auxIntersectionPoint;
		delete auxNormalIntersection;
		return color;
	}
}

Ray * depthOfFieldRay(float x, float y) {
	//create random angle (lens)
	float random = (float)rand() / (float)RAND_MAX;
	float angle = random * 2 * 3.14f;

	//coordenadas do ponto na lente
	glm::vec3 lPoint = glm::vec3(scene->getViewpoint()->getW() * (random * glm::cos(angle) / scene->getViewpoint()->getXRes()) * aperture,
		scene->getViewpoint()->getH() * (random * sin(angle) / scene->getViewpoint()->getYRes()) * aperture,
		0.0f);

	//nova origem (coordenadas do ponto anterior no mundo)
	glm::vec3 newOrigin = lPoint.x * scene->getViewpoint()->getXVector() + lPoint.y * scene->getViewpoint()->getYVector() + *scene->getViewpoint()->getFromVector();

	//ponto no view plan
	glm::vec3 vPoint = glm::vec3(scene->getViewpoint()->getW() * (((x + 0.5f) / scene->getViewpoint()->getXRes()) - 0.5f),
		scene->getViewpoint()->getH() * (((y + 0.5f) / scene->getViewpoint()->getYRes()) - 0.5f),
		-scene->getViewpoint()->getDf());

	//ponto no plano de foco
	glm::vec3 fPoint = glm::vec3(vPoint.x * (focalDistance / scene->getViewpoint()->getDf()),
		vPoint.y * (focalDistance / scene->getViewpoint()->getDf()),
		-focalDistance);

	//nova direcao
	glm::vec3 newDirection = glm::normalize(glm::vec3((fPoint.x - lPoint.x) * scene->getViewpoint()->getXVector() +
		(fPoint.y - lPoint.y) * scene->getViewpoint()->getYVector() +
		(fPoint.z - lPoint.z) * scene->getViewpoint()->getZVector()));

	Ray * newRay = new Ray();
	newRay->setOrigin(newOrigin.x, newOrigin.y, newOrigin.z);
	newRay->setDirection(newDirection);
	return newRay;
}

/////////////////////////////////////////////////////////////////////// ERRORS

bool isOpenGLError() {
	bool isError = false;
	GLenum errCode;
	const GLubyte *errString;
	while ((errCode = glGetError()) != GL_NO_ERROR) {
		isError = true;
		errString = gluErrorString(errCode);
		std::cerr << "OpenGL ERROR [" << errString << "]." << std::endl;
	}
	return isError;
}

void checkOpenGLError(std::string error)
{
	if (isOpenGLError()) {
		std::cerr << error << std::endl;
		exit(EXIT_FAILURE);
	}
}

/////////////////////////////////////////////////////////////////////// SHADERs
/////////////////////////////////////////////////////////////////////// SHADERs
const GLchar* VertexShader =
{
	"#version 330 core\n"

	"in vec2 in_Position;\n"
	"in vec3 in_Color;\n"
	"uniform mat4 Matrix;\n"
	"out vec4 color;\n"

	"void main(void)\n"
	"{\n"
	"	vec4 position = vec4(in_Position, 0.0, 1.0);\n"
	"	color = vec4(in_Color, 1.0);\n"
	"	gl_Position = Matrix * position;\n"

	"}\n"
};

const GLchar* FragmentShader =
{
	"#version 330 core\n"

	"in vec4 color;\n"
	"out vec4 out_Color;\n"

	"void main(void)\n"
	"{\n"
	"	out_Color = color;\n"
	"}\n"
};

void createShaderProgram()
{
	VertexShaderId = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(VertexShaderId, 1, &VertexShader, 0);
	glCompileShader(VertexShaderId);

	FragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(FragmentShaderId, 1, &FragmentShader, 0);
	glCompileShader(FragmentShaderId);

	ProgramId = glCreateProgram();
	glAttachShader(ProgramId, VertexShaderId);
	glAttachShader(ProgramId, FragmentShaderId);

	glBindAttribLocation(ProgramId, VERTEX_COORD_ATTRIB, "in_Position");
	glBindAttribLocation(ProgramId, COLOR_ATTRIB, "in_Color");

	glLinkProgram(ProgramId);
	UniformId = glGetUniformLocation(ProgramId, "Matrix");

	checkOpenGLError("ERROR: Could not create shaders.");
}

void destroyShaderProgram()
{
	glUseProgram(0);
	glDetachShader(ProgramId, VertexShaderId);
	glDetachShader(ProgramId, FragmentShaderId);

	glDeleteShader(FragmentShaderId);
	glDeleteShader(VertexShaderId);
	glDeleteProgram(ProgramId);

	checkOpenGLError("ERROR: Could not destroy shaders.");
}
/////////////////////////////////////////////////////////////////////// VAOs & VBOs
void createBufferObjects()
{
	glGenVertexArrays(1, &VaoId);
	glBindVertexArray(VaoId);
	glGenBuffers(2, VboId);
	glBindBuffer(GL_ARRAY_BUFFER, VboId[0]);

	/* Não é necessário fazer glBufferData, ou seja o envio dos pontos para a placa gráfica pois isso
	é feito na drawPoints em tempo de execução com GL_DYNAMIC_DRAW */

	glEnableVertexAttribArray(VERTEX_COORD_ATTRIB);
	glVertexAttribPointer(VERTEX_COORD_ATTRIB, 2, GL_FLOAT, 0, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, VboId[1]);
	glEnableVertexAttribArray(COLOR_ATTRIB);
	glVertexAttribPointer(COLOR_ATTRIB, 3, GL_FLOAT, 0, 0, 0);

	// unbind the VAO
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	/*glDisableVertexAttribArray(VERTEX_COORD_ATTRIB);
	glDisableVertexAttribArray(COLOR_ATTRIB);*/
	checkOpenGLError("ERROR: Could not create VAOs and VBOs.");
}

void destroyBufferObjects()
{
	glDisableVertexAttribArray(VERTEX_COORD_ATTRIB);
	glDisableVertexAttribArray(COLOR_ATTRIB);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	glDeleteBuffers(1, VboId);
	glDeleteVertexArrays(1, &VaoId);
	checkOpenGLError("ERROR: Could not destroy VAOs and VBOs.");
}

void drawPoints()
{
	glBindVertexArray(VaoId);
	glUseProgram(ProgramId);

	glBindBuffer(GL_ARRAY_BUFFER, VboId[0]);
	glBufferData(GL_ARRAY_BUFFER, size_vertices, vertices, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, VboId[1]);
	glBufferData(GL_ARRAY_BUFFER, size_colors, colors, GL_DYNAMIC_DRAW);

	glUniformMatrix4fv(UniformId, 1, GL_FALSE, m);
	glDrawArrays(GL_POINTS, 0, RES_X*RES_Y);
	glFinish();

	glUseProgram(0);
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	checkOpenGLError("ERROR: Could not draw scene.");
}

/////////////////////////////////////////////////////////////////////// CALLBACKS

// Render function by primary ray casting from the eye towards the scene's objects

void renderScene()
{
	std::cout << "\nANTIALIASING: " << std::boolalpha << ANTIALIASING << std::endl;
	std::cout << "SOFT SHADOWS: " << std::boolalpha << softShadows << std::endl;
	std::cout << "USING GRID: " << std::boolalpha << withGrid << std::endl;
	std::cout << "USING DEPTH OF FIELD: " << std::boolalpha << depthOfField << std::endl;

	start = std::clock();

	int index_pos=0;
	int index_col=0;

	//para cada pixel
	for (int y = 0; y < RES_Y; y++)
	{
		for (int x = 0; x < RES_X; x++)
		{
			//Debug helper
			if (x == 100 && y == 150) PIXELTEST = true;
			else PIXELTEST = false;
			
			//Color at the pixel
			glm::vec3 color(0);

			if (!ANTIALIASING) {
				if (!depthOfField) {
					//primary ray
					Ray * ray = calculatePrimaryRay(x, y);
					//result color of the ray
					Color * color_aux = rayTracing(ray, 0, false);
					color += glm::vec3(color_aux->getR(), color_aux->getG(), color_aux->getB());
					delete color_aux;
					delete ray;
				}
				else {
					for (int i = 0; i < dof_N; i++) {
						Ray * newRay = depthOfFieldRay(x, y);
						Color * color_aux = rayTracing(newRay, 0, false);
						color += glm::vec3(color_aux->getR(), color_aux->getG(), color_aux->getB());
						delete newRay;
						delete color_aux;
					}
					color /= dof_N;
				}
			}
			else {
				if (!depthOfField) {
					//Apply stochastic random sampling - JITTERING
					for (int p = 0; p < ANTIALIASING_N; p++) {
						for (int q = 0; q < ANTIALIASING_N; q++) {
							float random = rand() % 1; //random number between 0 and 1
							Ray * ray = calculatePrimaryRay(x + (p + random) / ANTIALIASING_N, y + (q + random) / ANTIALIASING_N);
							Color * color_aux = rayTracing(ray, 0, false);
							color += glm::vec3(color_aux->getR(), color_aux->getG(), color_aux->getB());
							delete color_aux;
							delete ray;
						}
					}
					color /= glm::pow(ANTIALIASING_N, 2);
				}
				else {
					for (int p = 0; p < ANTIALIASING_N; p++) {
						for (int q = 0; q < ANTIALIASING_N; q++) {
							float random = rand() % 1; //random number between 0 and 1
							glm::vec3 auxColor = glm::vec3();
							for (int i = 0; i < dof_N; i++) {
								Ray * newRay = depthOfFieldRay(x + (p + random) / ANTIALIASING_N, y + (q + random) / ANTIALIASING_N);
								Color * color_aux = rayTracing(newRay, 0, false);
								auxColor += glm::vec3(color_aux->getR(), color_aux->getG(), color_aux->getB());
								delete newRay;
								delete color_aux;
							}
							auxColor /= dof_N;
							color += auxColor;
						}
					}
					color /= glm::pow(ANTIALIASING_N, 2);
				}
			}

		    vertices[index_pos++]= (float)x;
			vertices[index_pos++]= (float)y;
			colors[index_col++]= (float)color.x;
			colors[index_col++]= (float)color.y;
			colors[index_col++] = (float)color.b;

			if(draw_mode == 0) {  // desenhar o conteúdo da janela ponto a ponto
				drawPoints();
				index_pos=0;
				index_col=0;
			}
		}
		//printf("line %d", y);
		if(draw_mode == 1) {  // desenhar o conteúdo da janela linha a linha
				drawPoints();
				index_pos=0;
				index_col=0;
		}
	}

	if(draw_mode == 2) //preenchar o conteúdo da janela com uma imagem completa
		 drawPoints();

	duration = (std::clock() - start) / (double)CLOCKS_PER_SEC;
	std::cout << "RENDERING DURATION: " << duration << std::endl;
}

void cleanup()
{
	destroyShaderProgram();
	destroyBufferObjects();

	delete colors;
	delete vertices;
}

void ortho(float left, float right, float bottom, float top, 
			float nearp, float farp)
{
	m[0 * 4 + 0] = 2 / (right - left);
	m[0 * 4 + 1] = 0.0;
	m[0 * 4 + 2] = 0.0;
	m[0 * 4 + 3] = 0.0;
	m[1 * 4 + 0] = 0.0;
	m[1 * 4 + 1] = 2 / (top - bottom);
	m[1 * 4 + 2] = 0.0;
	m[1 * 4 + 3] = 0.0;
	m[2 * 4 + 0] = 0.0;
	m[2 * 4 + 1] = 0.0;
	m[2 * 4 + 2] = -2 / (farp - nearp);
	m[2 * 4 + 3] = 0.0;
	m[3 * 4 + 0] = -(right + left) / (right - left);
	m[3 * 4 + 1] = -(top + bottom) / (top - bottom);
	m[3 * 4 + 2] = -(farp + nearp) / (farp - nearp);
	m[3 * 4 + 3] = 1.0;
}

void reshape(int w, int h)
{
    glClear(GL_COLOR_BUFFER_BIT);
	glViewport(0, 0, w, h);
	ortho(0, (float)RES_X, 0, (float)RES_Y, -1.0, 1.0);
}

void processKeys(unsigned char key, int xx, int yy)
{
	switch (key) {

	case 97: //a - switch antialiasing on/off
		ANTIALIASING = !ANTIALIASING;
		break;
	case 115: //s - switch softShadows on/off
		softShadows = !softShadows;
		break;
	case 103: //g - use/dont use grid
		withGrid = !withGrid;
		break;
	case 100: //d - depth of field on/off
		depthOfField = !depthOfField;
		break;
	}
	renderScene();
}

/////////////////////////////////////////////////////////////////////// SETUP
void setupCallbacks()
{
	glutCloseFunc(cleanup);
	glutDisplayFunc(renderScene);
	glutReshapeFunc(reshape);
}

void setupGLEW() {
	glewExperimental = GL_TRUE;
	GLenum result = glewInit();
	if (result != GLEW_OK) {
		std::cerr << "ERROR glewInit: " << glewGetString(result) << std::endl;
		exit(EXIT_FAILURE);
	}
	GLenum err_code = glGetError();
	printf("\n");
	printf("Vendor: %s\n", glGetString(GL_VENDOR));
	printf("Renderer: %s\n", glGetString(GL_RENDERER));
	printf("Version: %s\n", glGetString(GL_VERSION));
	printf("GLSL: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
}

void setupGLUT(int argc, char* argv[])
{
	glutInit(&argc, argv);

	glutInitContextVersion(3, 3);
	glutInitContextFlags(GLUT_FORWARD_COMPATIBLE);
	glutInitContextProfile(GLUT_CORE_PROFILE);

	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);

	glutInitWindowPosition(640, 100);
	glutInitWindowSize(RES_X, RES_Y);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);
	glDisable(GL_DEPTH_TEST);
	WindowHandle = glutCreateWindow(CAPTION);
	if (WindowHandle < 1) {
		std::cerr << "ERROR: Could not create a new rendering window." << std::endl;
		exit(EXIT_FAILURE);
	}
}

void init(int argc, char* argv[])
{
	setupGLUT(argc, argv);
	setupGLEW();
	std::cerr << "CONTEXT: OpenGL v" << glGetString(GL_VERSION) << std::endl;
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	createShaderProgram();
	createBufferObjects();
	setupCallbacks();
}

int main(int argc, char* argv[])
{
	scene = new Scene();
	//file name
	if (!(scene->load_nff("balls_high.nff"))) return 0;

	RES_X = scene->getViewpoint()->getXRes();
	RES_Y = scene->getViewpoint()->getYRes();
	
	if(draw_mode == 0) { // desenhar o conteúdo da janela ponto a ponto
		size_vertices = 2*sizeof(float);
		size_colors = 3*sizeof(float);
		printf("DRAWING MODE: POINT BY POINT\n");
	}
	else if(draw_mode == 1) { // desenhar o conteúdo da janela linha a linha
		size_vertices = 2*RES_X*sizeof(float);
		size_colors = 3*RES_X*sizeof(float);
		printf("DRAWING MODE: LINE BY LINE\n");
	}
	else if(draw_mode == 2) { // preencher o conteúdo da janela com uma imagem completa
		size_vertices = 2*RES_X*RES_Y*sizeof(float);
		size_colors = 3*RES_X*RES_Y*sizeof(float);
		printf("DRAWING MODE: FULL IMAGE\n");
	}
	else {
		printf("Draw mode not valid \n");
		exit(0);
	}
	printf("Resolucao X = %d  | Resolucao Y = %d.\n", RES_X, RES_Y);

	vertices = (float*)malloc(size_vertices);
    if (vertices==NULL) exit (1);

	colors = (float*)malloc(size_colors);
    if (colors==NULL) exit (1);

	/* initialize random seed: */
	srand(time(NULL));

	init(argc, argv);

	//	Mouse and Keyboard Callbacks
	glutKeyboardFunc(processKeys);

	std::cout << "\nPress 'a' to change antialiasing mode.\nPress 's' to change between soft/hard shadows.\nPress 'g' to use/don't use grid.\nPress 'd' to turn depth of field on/off." << std::endl;

	glutMainLoop();

	delete scene;

	exit(EXIT_SUCCESS);
}
///////////////////////////////////////////////////////////////////////