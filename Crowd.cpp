#include "Crowd.h"


void Crowd::generateBoids()
{
	srand(static_cast <unsigned> (time(0)));

	//(sizeof(flock)/sizeof(Boid))-1
	for (int i = 0; i < 20; i++) {
		Boid b(static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / 40))-20, static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / 40))-20);
		addBoid(b);
	}
}

void Crowd::renderBoids(mat4 gWVP, int shaderID)
{
	for (int i = 0; i < flock.size(); i++) {
		flock[i].renderBoid(gWVP, shaderID);
	}
}

void Crowd::setDeltaTime(float deltaTime1) {
    deltaTime = deltaTime1;
}

int Crowd::getSize()
{
	return flock.size();
}

Crowd::Crowd()
{
}

//Read only method that returns a copy of the Boid.
Boid Crowd::getBoid(int i)
{
	return flock[i];
}

//Read/write method that returns a reference of the Boid.
Boid& Crowd::getBoidPointer(int i)
{
	return flock[i];
}

// Runs the run function for every boid in the flock checking against the flock
// itself. Which in turn applies all the rules to the flock.
void Crowd::flocking()
{
	for (int i = 0; i < flock.size(); i++)
		flock[i].run(flock, deltaTime);
}