#pragma once
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "geometry.h"

class CModel
{
	float min_x, min_y, min_z;
	float max_x, max_y, max_z;
public: 
		// Import model with assimp  	
 	void load(const char* filename);
	 	// Calculate assimp node meshes
 	void calc_node(aiNode *node, const aiScene *scene);
		// Set up model filling meshes and triangles
 	void initialize();
		// Convert assimp mesh to CMesh
	SMesh getMesh(aiMesh *mesh);
		// Check box overflows and correct max,min values
	void checkBound(glm::vec3 vertice);
		// Initialize topleft and botright with bounds
 	void setBound();
 
public:
	std::vector<STriangle> triangles;
	std::vector<SMesh> meshes;
	vec3 topleft;
	vec3 botright;
	vec3 position;

	CModel(const char* filename, const vec3& pos = vec3(0.f,0.f,0.f));
};