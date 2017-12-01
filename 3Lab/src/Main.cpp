#include "Tracer.h"
#include "stdio.h"

#include "TinyObjLoader.h"
#include <vector>
#include <string>
#include <iostream>
using namespace std;


static bool TestLoadObj(const char* filename, const char* basepath = NULL,
                        bool triangulate = true) {
  std::cout << "Loading " << filename << std::endl;

  tinyobj::attrib_t attrib;
  std::vector<tinyobj::shape_t> shapes;
  std::vector<tinyobj::material_t> materials;

  std::string err;
  bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &err, filename,
                              basepath, triangulate);

  if (!err.empty()) {
    std::cerr << err << std::endl;
  }

  if (!ret) {
    printf("Failed to load/parse .obj.\n");
    return false;
  }

 // PrintInfo(attrib, shapes, materials);

  return true;
}


int main(int argc, char** argv)
{

  TestLoadObj("models/Flat.obj", "models/", false);

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

  tracer.m_pScene = &scene;
  tracer.RenderImage(xRes, yRes);
  tracer.SaveImageToFile("Result.png");
  return 0;
}