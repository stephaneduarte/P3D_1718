#ifndef __COLOR__
#define __COLOR__

class Color
{
private:
	//Componentes da cor
	float _r;
	float _g;
	float _b;

public:
	Color();
	Color(float r, float g, float b);
	~Color();

	//getters
	float getR();
	float getG();
	float getB();

	//setters
	void setColor(float r, float g, float b);
};

#endif