#pragma once
#include "scene.h"
#include "model.h"
#include "tracer.h"

using std::string;
using std::cerr;
using std::endl;

CModel::CModel(const char* filename, const vec3& pos)
{
    position = pos;
    float max = std::numeric_limits<float>::max();
    min_x = min_y = min_z = max;
    max_x = max_y = max_z = -max;
    load(filename);
}

void CModel::load(const char* filename)
{
    Assimp::Importer import;
    const aiScene *scene = import.ReadFile(filename, aiProcess_Triangulate);    
    
    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) 
    {
        cerr << "Assimp import error: " << import.GetErrorString() << endl;
        return;
    }

    calc_node(scene->mRootNode, scene);
    initialize();
    setBound();
}

void CModel::calc_node(aiNode *node, const aiScene *scene)
{
    for(uint i = 0; i < node->mNumMeshes; ++i){
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]]; 
        meshes.push_back(getMesh(mesh));            
    }
        // same for children
    for(uint i = 0; i < node->mNumChildren; ++i){
        calc_node(node->mChildren[i], scene);
    }
}  

SMesh CModel::getMesh(aiMesh *mesh)
{
    std::vector<vec3> vertices;
    std::vector<vec3> normals;

    for(uint i = 0; i < mesh->mNumVertices; ++i){
        vec3 v;
        v.x = mesh->mVertices[i].x;
        v.y = mesh->mVertices[i].y;
        v.z = mesh->mVertices[i].z;
        v += position; 
        vertices.push_back(v);
        
        v.x = mesh->mNormals[i].x;
        v.y = mesh->mNormals[i].y;
        v.z = mesh->mNormals[i].z;
        normals.push_back(v);
    }
    SMesh res = {vertices, normals};
    return res;
}  

void CModel::initialize()
{
    for(size_t i = 0; i < meshes.size(); ++i){
        for(size_t j = 0; j < meshes[i].m_vertices.size(); j+=3){
            vec3 a = meshes[i].m_vertices[j];
            vec3 b = meshes[i].m_vertices[j+1];
            vec3 c = meshes[i].m_vertices[j+2];
            checkBound(a);
            checkBound(b);
            checkBound(c);
            STriangle temp(a, b, c, meshes[i].m_normals[j]);
            triangles.push_back(temp);
        }
    }
}

void CModel::checkBound(vec3 vertice)
{
    min_x = vertice.x < min_x ? vertice.x : min_x;
    min_y = vertice.y < min_y ? vertice.y : min_y;
    min_z = vertice.z < min_z ? vertice.z : min_z;
    
    max_x = vertice.x > max_x ? vertice.x : max_x;
    max_y = vertice.y > max_y ? vertice.y : max_y;
    max_z = vertice.z > max_z ? vertice.z : max_z;
}

void CModel::setBound()
{
    topleft = vec3(min_x, max_y, max_z);
    botright = vec3(max_x, min_y, min_z);
} 