#include <vector>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <math.h>
#include "maths_funcs.h"
#include "Seal.h"
#include <random>


class Boid {
public:
    Seal seal;
    vec3 location;
    vec3 velocity;
    vec3 acceleration;
    float maxSpeed;
    float maxForce;
    Boid() {}
    Boid(float x, float y);
    void renderBoid(mat4 gWVP, int shaderID);
    void updateSeal(vec3 location, vec3 velocity);
    void applyForce(const vec3& force);
    // Three Laws that boids follow
    vec3 Separation(const std::vector<Boid>& Boids);
    vec3 Alignment(const std::vector<Boid>& Boids);
    vec3 Cohesion(const std::vector<Boid>& Boids);
    //Functions involving SFML and visualisation linking
    vec3 seek(const vec3& v);
    void run(const std::vector<Boid>& v, float delta);
    void update();
    void flock(const std::vector<Boid>& v);
    void borders();
    float angle(const vec3& v);
};

