#pragma once

#include "glm/glm.hpp"
#include <vector>
#include <string>

struct SRay
{
  glm::vec3 m_start;
  glm::vec3 m_dir;
};

struct SCamera
{
  glm::vec3 m_pos;          // Camera position and orientation
  glm::vec3 m_forward;      // Orthonormal basis
  glm::vec3 m_right;
  glm::vec3 m_up;

  glm::vec2 m_viewAngle;    // View angles, rad
  glm::uvec2 m_resolution;  // Image resolution: w, h

  std::vector<glm::vec3> m_pixels;  // Pixel array
};

struct SMesh
{
  std::vector<glm::vec3> m_vertices;  // vertex positions
  std::vector<glm::uvec3> m_triangles;  // vetrex indices
};

struct SLight
{
  glm::vec3 m_pos;
  uint      m_intensity;
};

class CScene
{
private:
  //std::vector<Figure*> figures;
  SLight Scene_light;
  SCamera Scene_camera;
  glm::vec3 Top_right;
  glm::vec3 Bot_left;
public:
  //CModel* model;
  CScene(std::string path);
  //CScene(CModel& loaded_model);
  //void LoadModel(const char* path);
  //void SaveImageToFile(const Image &im, const char *path);
  //Image LoadImageFromFile(const char *path);
  //void PlaceRouter();
};