#include "Types.h"
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

void loadModel(std::string);
void processNodeInScene(aiNode, const aiScene);
void processMeshInScene(aiMesh);

CScene::CScene(std::string path) {
  Top_right = glm::vec3(std::numeric_limits<float>::max(),
                        std::numeric_limits<float>::max(),
                        std::numeric_limits<float>::max());
  Bot_left = glm::vec3(-std::numeric_limits<float>::max(),
                       -std::numeric_limits<float>::max(),
                       -std::numeric_limits<float>::max());

  loadModel(path);

}

void loadModel(std::string path)
{
    Assimp::Importer import;
    const aiScene* scene = import.ReadFile(path, aiProcess_Triangulate);
    
    if(!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        printf("Error importing model: %s\n", import.GetErrorString());
    } else {
        processNodeInScene(scene->mRootNode, scene);
    }
}

void processNodeInScene(aiNode *node, const aiScene *scene)
{
    // Process all the node's meshes (if any)
    for(uint i = 0; i < node->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        mesh.push_back(processMeshInScene(mesh));
    }
    
    // Then do the same for each of its children
    for(uint i = 0; i < node->mNumChildren; i++) {
        processNodeInScene(node->mChildren[i], scene);
    }
}  

SMesh processMeshInScene(aiMesh *mesh)
{
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> normals;

    for(uint i = 0; i < mesh->mNumVertices; i++){
      v3 vector;
      vector.x = mesh->mVertices[i].x;
      vector.y = mesh->mVertices[i].y;
      vector.z = mesh->mVertices[i].z;
      vertices.push_back(vector);

      vector.x = mesh->mNormals[i].x;
      vector.y = mesh->mNormals[i].y;
      vector.z = mesh->mNormals[i].z;
      normals.push_back(vector);
    }
    SMesh mesh;
    mesh.m_verticies = vertices;
    mesh.m_normals   = normals;
    return mesh;
}  