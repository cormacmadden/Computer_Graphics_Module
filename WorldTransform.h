#pragma once
#include "maths_funcs.h";

class WorldTrans {
public:
	WorldTrans() {}

	void SetScale(float scale); 
	void SetRoatation(float x, float y, float z);
	void SetPosition(float x, float y, float z);

	void Rotate(float x, float y, float z);

	mat4 GetMatrix();
private:
	float m_scale = 1.0f;
	vec3 m_rotation = vec3(0.0f, 0.0f, 0.0f);
	vec3 m_position = vec3(0.0f, 0.0f, 0.0f);
};