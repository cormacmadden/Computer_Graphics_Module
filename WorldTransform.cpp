#include "WorldTransform.h"
#include <stdlib.h>

void WorldTrans::SetScale(float scale)
{
	m_scale = scale;
}

void WorldTrans::SetRoatation(float x, float y, float z)
{
	m_rotation.v[0] = x;
	m_rotation.v[1] = y;
	m_rotation.v[2] = z;
}

void WorldTrans::SetPosition(float x, float y, float z)
{
	m_position.v[0] = x;
	m_position.v[1] = y;
	m_position.v[2] = z;
}

void WorldTrans::Rotate(float x, float y, float z)
{
	m_rotation.v[0] += x;
	m_rotation.v[1] += y;
	m_rotation.v[2] += z;
}

mat4 WorldTrans::GetMatrix()
{
	mat4 scale = mat4(m_scale, 0.0f, 0.0f, 0.0f,
				0.0f, m_scale, 0.0f, 0.0f,
				0.0f, 0.0f, m_scale, 0.0f,
				0.0f, 0.0f, 0.0f, 1.0f);

	mat4 rotate = initRotateMatrix(m_rotation.v[0], m_rotation.v[2], m_rotation.v[3]);

	mat4 temp = identity_mat4();
	mat4 translation = translate(temp, vec3(0.0f, 0.0f, 0.0f));
	mat4 worldTransformation = translation * rotate * scale;
	return worldTransformation;
}
