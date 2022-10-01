#include "Boid.h"

#define PI 3.141592635
#define w_height 20000
#define w_width 20000
// =============================================== //
// ======== Boid Functions from Boid.h =========== //
// =============================================== //

Boid::Boid(float x, float y)
{

    acceleration = vec3(0.0f,0.0f,0.0f);
    velocity = vec3(static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / 2))-1, 0.0f, static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / 2))-1);
    location = vec3(x, 0.0f, y);
    maxSpeed = 1.5;
    maxForce = 0.5;
    seal = Seal(vec3(x , 0.0f, y));
}

void Boid::renderBoid(mat4 gWVP, int shaderID)
{
        seal.renderSeal(gWVP, shaderID);
}

// Adds force Pvector to current force Pvector
void Boid::applyForce(const vec3& force)
{
    acceleration += force;
}

// Separation
// Keeps boids from getting too close to one another
vec3 Boid::Separation(const std::vector<Boid>& boids)
{
    // Distance of field of vision for separation between boids
    float desiredseparation = 3;
    vec3 steer(0, 0, 0);
    int count = 0;
    // For every boid in the system, check if it's too close
    for (int i = 0; i < boids.size(); i++) {
        // Calculate distance from current boid to boid we're looking at
        float d = length(location - boids[i].location);
        // If this is a fellow boid and it's too close, move away from it
        if ((d > 0) && (d < desiredseparation)) {
            vec3 diff(0, 0, 0);
            diff = location - boids[i].location;
            diff = normalise(diff);
            diff = diff / d;
            steer += diff;
            count++;
            steer.v[1] = 0;
        }
    }
    // Adds average difference of location to acceleration
    if (count > 0)
        steer = steer / (float)count;
    if (length(steer) > 0) {
        // Steering = Desired - Velocity
        steer = normalise(steer);
        steer *= maxSpeed;
        steer -= (velocity);
        float size = length(steer);
        if (size > maxForce) {
            steer = vec3(steer.v[0] / size, 0.0f, steer.v[2] / size);
        }
    }
    return steer;
}

// Alignment
// Calculates the average velocity of boids in the field of vision and
// manipulates the velocity of the current boid in order to match it
vec3 Boid::Alignment(const std::vector<Boid>& Boids)
{
    float neighbordist = 7; // Field of vision

    vec3 sum(0, 0,0);
    int count = 0;
    for (int i = 0; i < Boids.size(); i++) {
        float d = length(location - (Boids[i].location));
        if ((d > 0) && (d < neighbordist)) { // 0 < d < 50
            sum += Boids[i].velocity;
            count++;
        }
    }
    // If there are boids close enough for alignment...
    if (count > 0) {
        sum= sum /(float)count;// Divide sum by the number of close boids (average of velocity)
        sum=normalise(sum);            // Turn sum into a unit vector, and
        sum*=maxSpeed;    // Multiply by maxSpeed
        // Steer = Desired - Velocity
        vec3 steer;
        steer = sum - velocity; //sum = desired(average)
        float size = length(steer);
        if (size > maxForce) {
            steer = vec3(steer.v[0] / size, 0.0f, steer.v[2] / size);
        }
        return steer;
    }
    else {
        vec3 temp(0, 0,0);
        return temp;
    }
}

// Cohesion
// Finds the average location of nearby boids and manipulates the
// steering force to move in that direction.
vec3 Boid::Cohesion(const std::vector<Boid>& Boids)
{
    float neighbordist = 5;
    vec3 sum(0, 0, 0);
    int count = 0;
    for (int i = 0; i < Boids.size(); i++) {
        float d = length(location - (Boids[i].location));
        if ((d > 0) && (d < neighbordist)) {
            sum += (Boids[i].location);
            count++;
        }
    }
    if (count > 0) {
        sum = sum / count;
        return seek(sum);
    }
    else {
        vec3 temp(0, 0,0);
        return temp;
    }
}

// Limits the maxSpeed, finds necessary steering force and
// normalizes vectors
vec3 Boid::seek(const vec3& v)
{
    vec3 desired;
    desired -= v;  // A vector pointing from the location to the target
    // Normalize desired and scale to maximum speed
    desired = normalise(desired);
    desired *= maxSpeed ;
    // Steering = Desired minus Velocity
    vec3 steer;
    steer = desired - velocity;
    float size = length(acceleration);
    if (size > maxForce) {
        steer = vec3(steer.v[0]/size, 0.0f, steer.v[2]/size);
    }
    //acceleration.limit(maxForce);  // Limit to maximum steering force
    steer.v[1] = 0;
    return steer;
}

// Modifies velocity, location, and resets acceleration with values that
// are given by the three laws.
void Boid::update()
{
    //To make the slow down not as abrupt
    acceleration *= 0.4;
    // Update velocity
    velocity += acceleration;
    // Limit speed
    float size = length(velocity);
    if (size > maxSpeed) {
        velocity = vec3(velocity.v[0] / size, 0.0f, velocity.v[2] / size);
    }
    //velocity.limit(maxSpeed);

    seal.Pos = location;
    velocity = normalise(velocity);
    seal.Yaw = direction_to_heading(velocity)-270.0f;
    seal.Front.v[0] = velocity.v[0];
    seal.Front.v[2] = velocity.v[2];

    seal.Front = normalise(seal.Front);

    location += velocity*0.1f;
    // Reset accelertion to 0 each cycle
    acceleration *= 0;

}

// Run flock() on the flock of boids.
// This applies the three rules, modifies velocities accordingly, updates data,
// and corrects boids which are sitting outside of the SFML window
void Boid::run(const std::vector<Boid>& v, float delta)
{
    seal.calculateFlap(delta);
    flock(v);
    update();
    borders();
}

// Applies the three laws to the flock of boids
void Boid::flock(const std::vector<Boid>& v)
{
    vec3 sep = Separation(v);
    vec3 ali = Alignment(v);
    vec3 coh = Cohesion(v);
     //Arbitrarily weight these forces
    sep *= (0.3);
    ali *= (0.025); // Might need to alter weights for different characteristics
    coh *= (0.0025);
    // Add the force vectors to acceleration
    applyForce(sep);
    applyForce(ali);
    applyForce(coh);
    
    //applyForce(vec3(1.0f,0.0f,0.0f));
}

// Checks if boids go out of the window and if so, wraps them around to
// the other side.

void Boid::borders()
{
    /*
    if (location.v[0] < -1000)    location.v[0] += w_width;
    if (location.v[2] < -1000)    location.v[2] += w_height;
    if (location.v[0] > 1000)     location.v[0] -= w_width;
    if (location.v[2] > 1000)     location.v[2] -= w_height;

    if (location.v[0] < -20)    location.v[0] = 20;
    if (location.v[2] < -20)    location.v[2] = 20;
    if (location.v[0] > 20)     location.v[0] = -20;
    if (location.v[2] > 20)     location.v[2] = -20;
        */
    if (location.v[0] < -20)    applyForce(vec3(1.0,0.0,0.0));
    if (location.v[2] < -20)    applyForce(vec3(1.0,0.0,1.0));
    if (location.v[0] > 20)     applyForce(vec3(-1.0,0.0,0.0));
    if (location.v[2] > 20)     applyForce(vec3(0.0,0.0,-1.0));
}

// Calculates the angle for the velocity of a boid which allows the visual
// image to rotate in the direction that it is going in.
float Boid::angle(const vec3& v)
{
    // From the definition of the dot product
    float angle = (float)(atan2(v.v[0], -v.v[2]) * 180 / PI);
    return angle;
}

