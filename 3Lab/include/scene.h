#pragma once
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "geometry.h"
#include "model.h"

class CScene
{
public:
	std::vector<Figure*> figures;
	std::vector<SLight> lights;
	SCamera cameras;
	void save_image(const Image &im, const char *path);

public:
	CScene(CModel& loaded_model, SCamera Cameras, SLight Lights, SRouter& Router);
};
