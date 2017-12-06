#pragma once
#include <iostream>
#include <vector>
#include "geometry.h"
#include "tracer.h"
#include "types.h"
#include "model.h"

int main(int argc, char** argv)
{
    int xRes = 800;  // Default resolution
    int yRes = 600;

    if(argc == 3) // There is input file in parameters
    {
      FILE* file = fopen(argv[2], "r");
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

    CModel     flat("../../models/new.obj");  
    SCamera    camera(vec3(0, 20, 0), vec3(0, 0, -1), vec3(1, 0, 0), PI/2);
    SLight     light(vec3(0, 20, 0), 10);
    SRouter    router(vec3(-6, 0, 0), 0.2, vec3(1, 0, 0), 10000);
    CScene     scene(flat, camera, light, router);
    SVoxelGrid grid(flat.topleft, flat.botright); 

    router.fill_grid(scene.figures, grid, 20000);
    grid.BoxFilter();

      /* Ray Tracer render setting up */
    CTracer tracer_top(camera, &scene, grid);

    tracer_top.RenderImage(xRes, yRes, "top.bmp");
    return 0;
}