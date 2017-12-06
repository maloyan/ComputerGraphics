#pragma once
#include "scene.h"
#include "model.h"
#include "tracer.h"

using std::string;
using std::cerr;
using std::endl;

CScene::CScene(CModel& model, SCamera Cameras, SLight Lights, SRouter& Router)
{
    for(uint i = 0; i < model.triangles.size(); ++i)
        figures.push_back(&(model.triangles[i]));
    lights.push_back(Lights);
    cameras = Cameras;
    figures.push_back(&Router);
}

void CScene::save_image(const Image &im, const char *path)
{
    BMP out;
    out.SetSize(im.n_cols, im.n_rows);

    uint r, g, b;
    RGBApixel p;
    p.Alpha = 255;
    for (uint i = 0; i < im.n_rows; ++i) {
        for (uint j = 0; j < im.n_cols; ++j) {
            std::tie(r, g, b) = im(i, j);
            p.Red = r; p.Green = g; p.Blue = b;
            out.SetPixel(j, i, p);
        }
    }

    if (!out.WriteToFile(path))
        throw string("Error writing file ") + string(path);
}
