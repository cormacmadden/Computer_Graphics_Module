#pragma once

// Windows includes (For Time, IO, etc.)
#include <windows.h>
#include <mmsystem.h>
#include <iostream>
#include <string>
#include <stdio.h>
#include <math.h>
#include <vector> // STL dynamic memory.

// OpenGL includes
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <fstream>
#include <iostream>
#include <sstream>


// Assimp includes
#include <assimp/cimport.h> // scene importer
#include <assimp/scene.h> // collects data
#include <assimp/postprocess.h> // various extra operations

// Project includes
//#include "obj_parser.h"
#include "camera.h"
#include "maths_funcs.h"
//#include "obj_parser.h"
#include "Shader.h"

#define INVALID_MATERIAL 0xFFFFFFFF



class Mesh {
private:
	GLuint loc1, loc2, loc3;
	unsigned int vp_vbo, vn_vbo, vao, ebo, vt_vbo;


	enum BUFFER_TYPE {
		INDEX_BUFFER = 0,
		POS_VB = 1,
		TEXCOORD_VB = 2,
		NORMAL_VB = 3,
		WVP_MAT_VB = 4,
		WORLD_MAT_VB = 5,
		NUM_BUFFERS = 6
	};

	struct BasicMeshEntry {
		BasicMeshEntry()
		{
			NumIndices = 0;
			BaseVertex = 0;
			BaseIndex = 0;
			MaterialIndex = INVALID_MATERIAL;
		}

		unsigned int NumIndices;
		unsigned int BaseVertex;
		unsigned int BaseIndex;
		unsigned int MaterialIndex;
	};

	std::vector<BasicMeshEntry> m_Meshes;
	//std::vector<Material> m_Materials;

	std::vector<vec3> m_Positions;
	std::vector<vec3> m_Normals;
	std::vector<vec2> m_TexCoords;
	std::vector<unsigned int> m_Indices;

public:
	const char* name;
	vec3 transformMat;
	Mesh();
	Mesh(const aiMesh* mesh, vec3 transformation, const char* name);
	void draw(mat4 transform, GLuint matrix_location, GLuint texture, Shader shaderProgram);
	//void generateObjectBufferMesh(Shader shaderProgram);
};