#pragma once
#include "maths_funcs.h"
#include "Model.h"
#include "Utils.h"

class Seal
{
public:
	vec3 Pos;
	vec3 Front;
	vec3 Up;
	vec3 Right;
	vec3 WorldUp = vec3(0.0f,1.0f,0.0f);
	float Yaw = 0.0f;
	float Pitch = 0.0f;
	float MovementSpeed;
	Model seal;
	GLfloat rotate_y = 0.0f;
	GLfloat rotate_y_sin = 0.0f;
	//boid 


	Seal();
	Seal(vec3 position);
	void renderSeal(mat4 gWVP, int shaderID);
	void ProcessKeyboard(MovementDir direction, float deltaTime);
	void calculateFlap(float delta);
	void MoveSeal(int x, int y, float deltaTime);

	float convert(float degree) {
		float pi = M_PI;
		return (degree * (pi / 180));
	}

	//vec3 separation();
private:
	mat4 GetViewMatrix();
	void updateSealVectors();

};