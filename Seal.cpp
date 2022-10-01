#include "Seal.h"

Seal::Seal(vec3 position)
{
	seal = Model("D:\\Personal\\College\\Computer Graphics\\GitHub\\ComputerGraphicsProject\\Seal.obj");
	Pos = position;
	MovementSpeed = 5.0f;
	Front = vec3(1.0f, 0.0f, 0.0f);
	Right = vec3(0.0f, 0.0f, 1.0f);
}

Seal::Seal()
{
}

void Seal::renderSeal(mat4 pos,int shaderID) {

	//Transform the Seal
	mat4 seal_pos = identity_mat4();
	seal_pos = translate(seal_pos, vec3(0.0f, 0.0f, 0.0f));
	seal_pos = rotate_y_deg(seal_pos, Yaw);
	seal_pos = translate(seal_pos, Pos);

	int world_loc = glGetUniformLocation(shaderID, "world");
	glUniformMatrix4fv(world_loc, 1, GL_FALSE, seal_pos.m);
	seal.RenderMesh(seal_pos, shaderID, 0);
	seal.RenderMesh(seal_pos, shaderID, 4);
	seal.RenderMesh(seal_pos, shaderID, 3);
	seal.RenderMesh(seal_pos, shaderID, 6);
	seal.RenderMesh(seal_pos, shaderID, 7);

	mat4 tailMat = identity_mat4();
	tailMat = translate(tailMat, vec3(0.25, -0.05, 0.0f));
	tailMat = rotate_z_deg(tailMat, -rotate_y_sin);
	tailMat = translate(tailMat, vec3(-0.25, 0.05, 0.0f));
	tailMat = seal_pos * tailMat;

	glUniformMatrix4fv(world_loc, 1, GL_FALSE, tailMat.m);
	seal.RenderMesh(tailMat, shaderID, 2);

	mat4 wingMatL = identity_mat4();
	mat4 wingMatR = identity_mat4();
	wingMatL = rotate_x_deg(wingMatL, -rotate_y_sin);
	wingMatR = rotate_x_deg(wingMatR, rotate_y_sin);
	wingMatL = seal_pos * wingMatL;
	wingMatR = seal_pos * wingMatR;

	glUniformMatrix4fv(world_loc, 1, GL_FALSE, wingMatL.m);
	seal.RenderMesh(wingMatL, shaderID, 5);
	glUniformMatrix4fv(world_loc, 1, GL_FALSE, wingMatR.m);
	seal.RenderMesh(wingMatR, shaderID, 1);
}

void Seal::ProcessKeyboard(MovementDir direction, float deltaTime)
{
	float speed = MovementSpeed * deltaTime;
	if (direction == FORWARD)Pos += Front * speed;
	if (direction == BACKWARD)Pos -= Front * speed;
	if (direction == LEFT ){
		Yaw += 20.0f * speed;
	}
	if (direction == RIGHT) {
		Yaw -= 20.0f * speed;
	}
	updateSealVectors();
}

void Seal::MoveSeal(int x, int y,float deltaTime) {

	float speed = MovementSpeed * deltaTime;
	Pos += Front * speed;
	if (x == 1)Yaw += 20.0f * speed;
	if (x == -1)Yaw -= 20.0f * speed;
	updateSealVectors();

}

mat4 Seal::GetViewMatrix()
{
	return look_at(Pos, Pos + Front, Up);
}

void Seal::updateSealVectors()
{
	vec3 front;
	front.v[0] = cos(convert(-Yaw)) * cos(convert(Pitch));
	front.v[1] = sin(convert(Pitch));
	front.v[2] = sin(convert(-Yaw)) * cos(convert(Pitch));

	Front = normalise(front);
	Right = normalise(cross(Front, WorldUp));
	Up = normalise(cross(Right, Front));
}

void Seal::calculateFlap(float delta) {
	rotate_y += 500.0f * delta;
	rotate_y = fmodf(rotate_y, 360.0f);
	float rotate_y_rad = rotate_y * 3.14159 / 180;
	rotate_y_sin = sin(rotate_y_rad) * 4 - 1;
}


