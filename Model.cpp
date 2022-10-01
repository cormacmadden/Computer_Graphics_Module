// Windows includes (For Time, IO, etc.)
#include <windows.h>
#include <mmsystem.h>
#include <iostream>
#include <string>
#include <stdio.h>
#include <math.h>
#include <string>
#include <vector> // STL dynamic memory.

// OpenGL includes
#include <GL/glew.h>
#include <GL/freeglut.h>

// Assimp includes
#include <assimp/cimport.h> // scene importer
#include <assimp/Importer.hpp> // scene importer
#include <assimp/scene.h> // collects data
#include <assimp/postprocess.h> // various extra operations

// Project includes
#include "maths_funcs.h"
#include "Mesh.h"
#include "Model.h"
#include <cassert>
//#include "Material.h"
#define ASSIMP_LOAD_FLAGS (aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices)
#define ARRAY_SIZE_IN_ELEMENTS(a) (sizeof(a)/sizeof(a[0]))
#define POSITION_LOCATION  0
#define TEX_COORD_LOCATION 1
#define NORMAL_LOCATION    2
#define COLOR_TEXTURE_UNIT              GL_TEXTURE0
#define COLOR_TEXTURE_UNIT_INDEX		0
#define SPECULAR_EXPONENT_UNIT             GL_TEXTURE6

using namespace std;

Model::Model() {};

Model::Model(const char* file_name) {
	meshes = loadScene(file_name);
}

std::vector<Mesh> Model::loadScene(const char* file_name) {
	std::vector<Mesh> meshesTemp;
	Assimp::Importer Importer;

	glGenVertexArrays(1, &m_VAO);
	glBindVertexArray(m_VAO);

	glGenBuffers(ARRAY_SIZE_IN_ELEMENTS(m_Buffers), m_Buffers);

	const aiScene* scene = Importer.ReadFile(file_name, ASSIMP_LOAD_FLAGS);

	bool Ret = false;
	if (scene) {
		Ret = InitFromScene(scene, file_name);
	}
	glBindVertexArray(0);
	std::string name = std::string(file_name);
	size_t pos = 0;
	pos = name.find(".");
	std::string token = name.substr(0, pos);
	std::cout << "Model Name: " << token << '\n';
	std::cout << "Model Filename: " << name << '\n';
	vec3 trans = vec3(0.0f, 0.0f, 0.0f);


	//aiReleaseImport(scene);

	return meshesTemp;
}

void Model::countVerticesAndIndices(const aiScene* pScene, unsigned int& NumVertices, unsigned int& NumIndices)
{
	for (unsigned int i = 0; i < m_Meshes.size(); i++) {
		m_Meshes[i].MaterialIndex = pScene->mMeshes[i]->mMaterialIndex;
		m_Meshes[i].NumIndices = pScene->mMeshes[i]->mNumFaces * 3;
		m_Meshes[i].BaseVertex = NumVertices;
		m_Meshes[i].BaseIndex = NumIndices;
		NumVertices += pScene->mMeshes[i]->mNumVertices;
		NumIndices += m_Meshes[i].NumIndices;
	}
}

bool Model::InitFromScene(const aiScene* pScene, const char*& Filename)
{
	m_Meshes.resize(pScene->mNumMeshes);
	m_Materials.resize(pScene->mNumMaterials);

	unsigned int NumVertices = 0;
	unsigned int NumIndices = 0;

	countVerticesAndIndices(pScene, NumVertices, NumIndices);

	std::cout << "Num Vertices: " << NumVertices << '\n';
	std::cout << "Num Indices:" << NumIndices << '\n';

	ReserveSpace(NumVertices, NumIndices);

	InitAllMeshes(pScene);

	if (!InitMaterials(pScene, Filename)) {
		return false;
	}

	PopulateBuffers();

	return true;
}

void Model::ReserveSpace(unsigned int NumVertices, unsigned int NumIndices)
{
	m_Positions.reserve(NumVertices);
	m_Normals.reserve(NumVertices);
	m_TexCoords.reserve(NumVertices);
	m_Indices.reserve(NumIndices);
}

void Model::InitAllMeshes(const aiScene* pScene)
{
	for (unsigned int i = 0; i < m_Meshes.size(); i++) {
		const aiMesh* paiMesh = pScene->mMeshes[i];
		InitSingleMesh(paiMesh);
	}
}

void Model::InitSingleMesh(const aiMesh* paiMesh)
{
	const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);

	// Populate the vertex attribute vectors
	for (unsigned int i = 0; i < paiMesh->mNumVertices; i++) {
		const aiVector3D& pPos = paiMesh->mVertices[i];
		const aiVector3D& pNormal = paiMesh->mNormals[i];
		const aiVector3D& pTexCoord = paiMesh->HasTextureCoords(0) ? paiMesh->mTextureCoords[0][i] : Zero3D;

		m_Positions.push_back(vec3(pPos.x, pPos.y, pPos.z));
		m_Normals.push_back(vec3(pNormal.x, pNormal.y, pNormal.z));
		m_TexCoords.push_back(vec2(pTexCoord.x, pTexCoord.y));
	}

	// Populate the index buffer
	for (unsigned int i = 0; i < paiMesh->mNumFaces; i++) {
		const aiFace& Face = paiMesh->mFaces[i];
		assert(Face.mNumIndices == 3);
		m_Indices.push_back(Face.mIndices[0]);
		m_Indices.push_back(Face.mIndices[1]);
		m_Indices.push_back(Face.mIndices[2]);
	}
}

void Model::PopulateBuffers()
{
	glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[POS_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(m_Positions[0]) * m_Positions.size(), &m_Positions[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(POSITION_LOCATION);
	glVertexAttribPointer(POSITION_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[TEXCOORD_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(m_TexCoords[0]) * m_TexCoords.size(), &m_TexCoords[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(TEX_COORD_LOCATION);
	glVertexAttribPointer(TEX_COORD_LOCATION, 2, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[NORMAL_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(m_Normals[0]) * m_Normals.size(), &m_Normals[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(NORMAL_LOCATION);
	glVertexAttribPointer(NORMAL_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Buffers[INDEX_BUFFER]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(m_Indices[0]) * m_Indices.size(), &m_Indices[0], GL_STATIC_DRAW);

}

void Model::RenderModel(mat4 transform, unsigned int shaderID)
{
	glBindVertexArray(m_VAO);
	int matrix_location = glGetUniformLocation(shaderID, "world");
	//int alpha_loc = glGetUniformLocation(shaderID, "alpha");
	//glUniform1f(alpha_loc, 1.0f);
	for (unsigned int i = 0; i < m_Meshes.size(); i++) {

		unsigned int MaterialIndex = m_Meshes[i].MaterialIndex;
		
		//assert(MaterialIndex < m_Materials.size());
		
		int samplerLoc = glGetUniformLocation(shaderID, "gSampler");
		int materialpDiffuseLoc = glGetUniformLocation(shaderID, "gMaterial.pDiffuse");
		
		glUniform1i(samplerLoc, COLOR_TEXTURE_UNIT_INDEX);
		
		if (m_Materials[MaterialIndex].pDiffuse != NULL) {
			m_Materials[MaterialIndex].pDiffuse->Bind(COLOR_TEXTURE_UNIT);
			glUniform1i(materialpDiffuseLoc, 1);
		}
		
		else {
			glBindTexture(GL_TEXTURE_2D, 0); // << default texture object
			glUniform1i(materialpDiffuseLoc, 0);
		}

		if (m_Materials[MaterialIndex].pSpecularExponent) {
			m_Materials[MaterialIndex].pSpecularExponent->Bind(SPECULAR_EXPONENT_UNIT);
		}
		int materialAmbientColorLoc = glGetUniformLocation(shaderID, "gMaterial.AmbientColor");
		int materialDiffuseColorLoc = glGetUniformLocation(shaderID, "gMaterial.DiffuseColor");
		glUniform3f(materialDiffuseColorLoc, m_Materials[MaterialIndex].DiffuseColor.v[0], m_Materials[MaterialIndex].DiffuseColor.v[1], m_Materials[MaterialIndex].DiffuseColor.v[2]);
		glUniform3f(materialAmbientColorLoc, m_Materials[MaterialIndex].AmbientColor.v[0], m_Materials[MaterialIndex].AmbientColor.v[1], m_Materials[MaterialIndex].AmbientColor.v[2]);

		glUniformMatrix4fv(matrix_location, 1, GL_FALSE, transform.m);

		glDrawElementsBaseVertex(GL_TRIANGLES,
			m_Meshes[i].NumIndices,
			GL_UNSIGNED_INT,
			(void*)(sizeof(unsigned int) * m_Meshes[i].BaseIndex),
			m_Meshes[i].BaseVertex);
	}

	// Make sure the VAO is not changed from the outside
	glBindVertexArray(0);
}

void Model::RenderMesh(mat4 transform, unsigned int shaderID, int meshIndex)
{
	int matrix_location = glGetUniformLocation(shaderID, "world");
	glBindVertexArray(m_VAO);

	unsigned int MaterialIndex = m_Meshes[meshIndex].MaterialIndex;

	int samplerLoc = glGetUniformLocation(shaderID, "gSampler");
	glUniform1i(samplerLoc, COLOR_TEXTURE_UNIT_INDEX);

	int materialpDiffuseLoc = glGetUniformLocation(shaderID, "gMaterial.pDiffuse");

	if (m_Materials[MaterialIndex].pDiffuse != NULL) {
		m_Materials[MaterialIndex].pDiffuse->Bind(COLOR_TEXTURE_UNIT);
		glUniform1i(materialpDiffuseLoc, 1);
	}

		else {
			glBindTexture(GL_TEXTURE_2D, 0); // << default texture object
			glUniform1i(materialpDiffuseLoc, 0);
		}

		if (m_Materials[MaterialIndex].pSpecularExponent) {
			m_Materials[MaterialIndex].pSpecularExponent->Bind(SPECULAR_EXPONENT_UNIT);
		}
		int materialAmbientColorLoc = glGetUniformLocation(shaderID, "gMaterial.AmbientColor");
		int materialDiffuseColorLoc = glGetUniformLocation(shaderID, "gMaterial.DiffuseColor");
		glUniform3f(materialDiffuseColorLoc, m_Materials[MaterialIndex].DiffuseColor.v[0], m_Materials[MaterialIndex].DiffuseColor.v[1], m_Materials[MaterialIndex].DiffuseColor.v[2]);
		glUniform3f(materialAmbientColorLoc, m_Materials[MaterialIndex].AmbientColor.v[0], m_Materials[MaterialIndex].AmbientColor.v[1], m_Materials[MaterialIndex].AmbientColor.v[2]);

		glUniformMatrix4fv(matrix_location, 1, GL_FALSE, transform.m);

		glDrawElementsBaseVertex(GL_TRIANGLES,
			m_Meshes[meshIndex].NumIndices,
			GL_UNSIGNED_INT,
			(void*)(sizeof(unsigned int) * m_Meshes[meshIndex].BaseIndex),
			m_Meshes[meshIndex].BaseVertex);

	// Make sure the VAO is not changed from the outside
	glBindVertexArray(0);
}

string GetDirFromFilename(const string& Filename)
{
	// Extract the directory part from the file name
	string::size_type SlashIndex;
	SlashIndex = Filename.find_last_of("\\");
	string Dir;
	if (SlashIndex == -1) {
		Dir = ".";
	}
	else if (SlashIndex == 0) {
		Dir = "/";
	}
	else {
		Dir = Filename.substr(0, SlashIndex);
	}
	return Dir;
}

const Material& Model::GetMaterial()
{
	for (unsigned int i = 0; i < m_Materials.size(); i++) {
		if ((m_Materials[i].AmbientColor.v[0] != 0.0f) && (m_Materials[i].AmbientColor.v[1] != 0.0f) && (m_Materials[i].AmbientColor.v[2] != 0.0f)) {
			return m_Materials[i];
		}
	}
	return m_Materials[0];
}

void Model::LoadColors(const aiMaterial* pMaterial, int index)
{
	aiColor3D AmbientColor(0.0f, 0.0f, 0.0f);

	if (pMaterial->Get(AI_MATKEY_COLOR_AMBIENT, AmbientColor) == AI_SUCCESS) {
		printf("Loaded ambient color [%f %f %f]\n", AmbientColor.r, AmbientColor.g, AmbientColor.b);
		m_Materials[index].AmbientColor.v[0] = AmbientColor.r;
		m_Materials[index].AmbientColor.v[1] = AmbientColor.g;
		m_Materials[index].AmbientColor.v[2] = AmbientColor.b;
	}

	aiColor3D DiffuseColor(0.0f, 0.0f, 0.0f);

	if (pMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, DiffuseColor) == AI_SUCCESS) {
		printf("Loaded diffuse color [%f %f %f]\n", DiffuseColor.r, DiffuseColor.g, DiffuseColor.b);
		m_Materials[index].DiffuseColor.v[0] = DiffuseColor.r;
		m_Materials[index].DiffuseColor.v[1] = DiffuseColor.g;
		m_Materials[index].DiffuseColor.v[2] = DiffuseColor.b;
	}

	aiColor3D SpecularColor(0.0f, 0.0f, 0.0f);

	if (pMaterial->Get(AI_MATKEY_COLOR_SPECULAR, SpecularColor) == AI_SUCCESS) {
		printf("Loaded specular color [%f %f %f]\n", SpecularColor.r, SpecularColor.g, SpecularColor.b);
		m_Materials[index].SpecularColor.v[0] = SpecularColor.r;
		m_Materials[index].SpecularColor.v[1] = SpecularColor.g;
		m_Materials[index].SpecularColor.v[2] = SpecularColor.b;
	}
}

void Model::LoadTextures(const string& Dir, const aiMaterial* pMaterial, int index)
{
	LoadDiffuseTexture(Dir, pMaterial, index);
	LoadSpecularTexture(Dir, pMaterial, index);
}

void Model::LoadDiffuseTexture(const string& Dir, const aiMaterial* pMaterial, int index)
{
	m_Materials[index].pDiffuse = NULL;

	if (pMaterial->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
		aiString Path;

		if (pMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &Path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS) {
			string p(Path.data);

			if (p.substr(0, 2) == ".\\") {
				p = p.substr(2, p.size() - 2);
			}

			string FullPath = Dir + '\\' + p;

			m_Materials[index].pDiffuse = new Texture(GL_TEXTURE_2D, FullPath.c_str());

			if (!m_Materials[index].pDiffuse->Load()) {
				printf("Error loading diffuse texture '%s'\n", FullPath.c_str());
				exit(0);
			}
			else {
				printf("Loaded diffuse texture '%s'\n", FullPath.c_str());
			}
		}
	}
}

void Model::LoadSpecularTexture(const string& Dir, const aiMaterial* pMaterial, int index)
{
	m_Materials[index].pSpecularExponent = NULL;

	if (pMaterial->GetTextureCount(aiTextureType_SHININESS) > 0) {
		aiString Path;

		if (pMaterial->GetTexture(aiTextureType_SHININESS, 0, &Path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS) {
			string p(Path.data);

			if (p.substr(0, 2) == ".\\") {
				p = p.substr(2, p.size() - 2);
			}

			string FullPath = Dir + '/' + p;

			m_Materials[index].pSpecularExponent = new Texture(GL_TEXTURE_2D, FullPath.c_str());

			if (!m_Materials[index].pSpecularExponent->Load()) {
				printf("Error loading specular texture '%s'\n", FullPath.c_str());
				exit(0);
			}
			else {
				printf("Loaded specular texture '%s'\n", FullPath.c_str());
			}
		}
	}
}

bool Model::InitMaterials(const aiScene* pScene, const string& Filename)
{

	string Dir = GetDirFromFilename(Filename);

	bool Ret = true;

	// Initialize the materials
	for (unsigned int i = 0; i < pScene->mNumMaterials; i++) {
		const aiMaterial* pMaterial = pScene->mMaterials[i];

		LoadTextures(Dir, pMaterial, i);

		LoadColors(pMaterial, i);
	}

	return Ret;
}
