// Windows includes (For Time, IO, etc.)
#define NOMINMAX
#include <limits>
#include <windows.h>
#include <mmsystem.h>
#include <iostream>
#include <string>
#include <stdio.h>
#include <math.h>
#include <vector> // STL dynamic memory.
#include "Camera.h"
#include "Shader.h"
#include "Seal.h"
#include "Utils.h"
// OpenGL includes
#include <GL/glew.h>
#include <GL/freeglut.h>

// Assimp includes
#include <assimp/cimport.h> // scene importer
#include <assimp/scene.h> // collects data
#include <assimp/postprocess.h> // various extra operations

#include "Mesh.h"
#include "Model.h"
#include "Crowd.h"
// Project includes
//#include "maths_funcs.h"
//#include <glm/glm/glm.hpp>
//#include <glm/glm/gtc/matrix_transform.hpp>
//#include <glm/glm/gtc/type_ptr.hpp>
#include "maths_funcs.h"
/*----------------------------------------------------------------------------
MESH TO LOAD
----------------------------------------------------------------------------*/
// this mesh is a dae file format but you should be able to use any other format too, obj is typically what is used
// put the mesh in your project directory, or provide a filepath for it here

//test commit

#define MESH_NAME "monkeyhead_smooth.dae"
Camera camera;
int width = 1400;
int height = 700;
bool firstMouse = true;
GLfloat seal_forward = 0.0f;
GLfloat seal_right = 0.0f;
float delta;
float lastx = width / 2.0f;
float lasty = height / 2.0f;
float yaw = -90.0f;	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
float pitch = 0.0f;
bool keyStates[256];
float skyR = 0.6f;
float skyG = 0.87f;
float skyB = 0.96f;
bool trans = false;
bool skyMap = false;

using namespace std;

Texture* pTexture = NULL;
Texture* sphereTexture = NULL;
Shader myShader;
Shader skyShader;
Model terrain;
Model sphere;
Seal mySeal;
mat4 gWVP;
Crowd myCrowd;
Boid myBoid;

void display() {

	if (trans == true) {
		glDisable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	} //Transparancy
	else if (trans == false) {
		glDisable(GL_BLEND);
		glDisable(GL_CULL_FACE);
		glEnable(GL_DEPTH_TEST);
	}

	
	//glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"
	glClearColor(skyR, skyG, skyB, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Declare your uniform variables that will be used in your shader
	int view_loc = glGetUniformLocation(myShader.ID, "view");
	int proj_loc = glGetUniformLocation(myShader.ID, "proj");
	int view_loc1 = glGetUniformLocation(skyShader.ID, "view");
	int proj_loc1 = glGetUniformLocation(skyShader.ID, "proj");
	int skyColor_loc = glGetUniformLocation(myShader.ID, "skyColor");
	int light_dir_loc = glGetUniformLocation(myShader.ID, "gLight.Direction");
	int light_color_loc = glGetUniformLocation(myShader.ID, "lightColor");
	int light_pos_loc = glGetUniformLocation(myShader.ID, "lightPos");
	int view_pos_loc = glGetUniformLocation(myShader.ID, "viewPos");

	mat4 persp_proj = perspective(45.0f, (float)width / (float)height, 0.1f, 1000.0f);
	mat4 view = camera.GetViewMatrix();
	glUniformMatrix4fv(proj_loc, 1, GL_FALSE, persp_proj.m);
	glUniformMatrix4fv(view_loc, 1, GL_FALSE, view.m);
	glUniformMatrix4fv(proj_loc1, 1, GL_FALSE, persp_proj.m);
	glUniformMatrix4fv(view_loc1, 1, GL_FALSE, view.m);

	mat4 world = identity_mat4();
	if (skyMap == true) {
		skyShader.use();
		world = scale(world, vec3(2.0f, 2.0f, 2.0f));
		sphere.RenderModel(world * view, skyShader.ID);
	}
	vec3 lightCol = vec3(1.0f,1.0f,1.0f);
	vec3 lightPos = vec3(0.5f, 15.5f, 0.15f);
	//glUniform3fv(light_color_loc, 1, &lightCol[0]);

	glUniform3fv(light_pos_loc, 1, &lightPos.v[0]);
	glUniform3f(view_pos_loc, camera.Pos.v[0], camera.Pos.v[1], camera.Pos.v[2]);
	myShader.use();

	mat4 terrain_mat = identity_mat4();
	terrain_mat = scale(terrain_mat, vec3(10.0, 10.0, 10.0));
	terrain_mat = translate(terrain_mat, vec3(-10.0, 10.0, 10.0));
	terrain.RenderModel(terrain_mat, myShader.ID);

	myCrowd.renderBoids(world, myShader.ID);
	myCrowd.flocking();
	mySeal.renderSeal(world, myShader.ID);

	glutSwapBuffers();
}



void updateCamera() {
	if (keyStates['w'] == true) { //move cam forward
		camera.ProcessKeyboard(FORWARD, delta);
	}
	if (keyStates['a'] == true) {//mpve cam left
		camera.ProcessKeyboard(LEFT, delta);
	}
	if (keyStates['d'] == true) {//move cam right
		camera.ProcessKeyboard(RIGHT, delta);
	}
	if (keyStates['s'] == true) {//move cam backward
		camera.ProcessKeyboard(BACKWARD, delta);
	}
	if (keyStates['q'] == true) {//move cam backward
		camera.ProcessKeyboard(DOWN, delta);
	}
	if (keyStates['e'] == true) {//move cam backward
		camera.ProcessKeyboard(UP, delta);
	}
	if (keyStates['t'] == true) {//move cam backward
		trans = true;
	}
	if (keyStates['y'] == true) {//move cam backward
		trans = false;
	}
	if (keyStates['b'] == true) {//move cam backward
		skyMap = true;
	}
	if (keyStates['n'] == true) {//move cam backward
		skyMap = false;
	}
	if (keyStates['z'] == true) {//move light backward
		skyMap = false;
	}
}

void updateSeal() {
	if (keyStates['i'] == true) {
		mySeal.ProcessKeyboard(FORWARD, delta);
	}
	if (keyStates['k'] == true) {
		mySeal.ProcessKeyboard(BACKWARD, delta);
	}
	if (keyStates['l'] == true) {
		mySeal.ProcessKeyboard(RIGHT, delta);
	}
	if (keyStates['j'] == true) {
		mySeal.ProcessKeyboard(LEFT, delta);
	}
	mySeal.calculateFlap(delta);
}

void updateScene() {
	static DWORD last_time = 0;
	DWORD curr_time = timeGetTime();
	if (last_time == 0)
		last_time = curr_time;
	delta = (curr_time - last_time) * 0.001f;
	last_time = curr_time;

	myCrowd.setDeltaTime(delta);
	updateSeal();
	updateCamera();
	glutPostRedisplay();
}

void init()
{
	// Set up the models and shaders
	myShader = Shader("D:\\Personal\\College\\Computer Graphics\\GitHub\\ComputerGraphicsProject\\simpleVertexShader.txt","D:\\Personal\\College\\Computer Graphics\\GitHub\\ComputerGraphicsProject\\simpleFragmentShader.txt");
	skyShader = Shader("D:\\Personal\\College\\Computer Graphics\\GitHub\\ComputerGraphicsProject\\simpleVertexShader.txt", "D:\\Personal\\College\\Computer Graphics\\GitHub\\ComputerGraphicsProject\\skyShader.txt");
	mySeal = Seal(vec3(0.0f, 0.0f, 0.0f));
	terrain = Model("D:\\Personal\\College\\Computer Graphics\\GitHub\\ComputerGraphicsProject\\Terrain.obj");
	sphere = Model("D:\\Personal\\College\\Computer Graphics\\GitHub\\ComputerGraphicsProject\\SphereScene.obj");
	myCrowd.generateBoids();
	pTexture = new Texture(GL_TEXTURE_2D, "BlueTexture.jpeg");
	sphereTexture = new Texture(GL_TEXTURE_2D, "sunflowers_2k.hdr");

}

void mouseCallback(int x, int y) {

	if (firstMouse)
	{
		lastx = x;
		lasty = y;
		firstMouse = false;
	}

	static bool wrap = false;
	float dx = 0.0;
	float dy = 0.0;
	if (!wrap) {
		int ww = glutGet(GLUT_WINDOW_WIDTH);
		int wh = glutGet(GLUT_WINDOW_HEIGHT);

		dx = x - ww / 2;
		dy = y - wh / 2;

		// Do something with dx and dy here

		// move mouse pointer back to the center of the window
		wrap = true;
		glutWarpPointer(ww / 2, wh / 2);
	}
	else {
		wrap = false;
	}

	float sensitivity = 0.1f;
	dx *= sensitivity;
	dy *= sensitivity;

	yaw += dx;
	pitch -= dy;

	if (pitch > 89.0f)
		pitch = 89.0f;

	if (pitch < -89.0f)
		pitch = -89.0f;

	camera.ProcessMouseMovement(dx, -dy);
}
// Placeholder code for the keypress
void keypress(unsigned char key, int x, int y) {

}

void keyDown(unsigned char key, int x, int y) {
	keyStates[key] = true;
}

void keyUp(unsigned char key, int x, int y) {
	keyStates[key] = false;
}

int main(int argc, char** argv) {

	// Set up the window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(width, height);
	glutCreateWindow("Hello Triangle");

	// Tell glut where the display function is
	glutDisplayFunc(display);
	glutIdleFunc(updateScene);
	glutKeyboardFunc(keyDown);
	glutKeyboardUpFunc(keyUp);
	glutPassiveMotionFunc(mouseCallback);
	glutSetCursor(GLUT_CURSOR_NONE);

	// A call to glewInit() must be done after glut is initialized!
	GLenum res = glewInit();
	// Check for any errors
	if (res != GLEW_OK) {
		fprintf(stderr, "Error: '%s'\n", glewGetErrorString(res));
		return 1;
	}
	// Set up your objects and shaders
	init();
	// Begin infinite event loop
	glutMainLoop();
	return 0;
}