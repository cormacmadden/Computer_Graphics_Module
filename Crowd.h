#pragma once
#include "Seal.h"
#include "maths_funcs.h"
#include "Boid.h"

class Crowd {
public:
	Crowd();
	std::vector<Boid> flock;
	//Boid flock[5];
	Boid getBoid(int i);
	Boid &getBoidPointer(int i);
	// Mutator Functions
	void flocking();
	int getSize();

	float fov;
	float radius;
	float deltaTime;
	void generateBoids();
	void calcNewPos();
	void calcNewDir();
	void renderBoids(mat4 gWVP, int shaderID);
	void setDeltaTime(float deltaTime);
private:

	void addBoid(const Boid& b)
	{
		flock.push_back(b);
	}
};

