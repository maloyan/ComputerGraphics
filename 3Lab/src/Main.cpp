#include "Tracer.h"
#include "stdio.h"

#include <vector>
#include <string>
#include <iostream>
using namespace std;


int main(int argc, char** argv)
{
  SCamera camera;
  SLight  light;
  CTracer tracer;
  CScene scene;
  int xRes = 1024;  // Default resolution
  int yRes = 768;

  if(argc == 2) // There is input file in parameters
  {
    FILE* file = fopen(argv[1], "r");
    if(file)
    {
      int xResFromFile = 0;
      int yResFromFile = 0;
      if(fscanf(file, "%d %d", &xResFromFile, &yResFromFile) == 2)
      {
        xRes = xResFromFile;
        yRes = yResFromFile;
      }
      else
        printf("Invalid config format! Using default parameters.\r\n");

      fclose(file);
    }
    else
      printf("Invalid config path! Using default parameters.\r\n");
  }
  else
    printf("No config! Using default parameters.\r\n");

  camera.m_pos   = glm::vec3(0, 0, 0);
  camera.m_right = glm::vec3(0, 0, 0);
  camera.m_up    = glm::vec3(0, 0, 0);

  light.m_pos    = glm::vec3(0, 0, 0);
  light.m_intensity = 0;

  tracer.m_pScene = &scene;
  tracer.RenderImage(xRes, yRes);
  tracer.SaveImageToFile("Result.png");
  return 0;
}