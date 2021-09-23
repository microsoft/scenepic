// ----------------------------------------------------------------------------
//
// audio_tracks.cpp -- ScenePic audio track example app
//
// Copyright (C) 2020 Microsoft
//
// For conditions of distribution and use, see copyright notice in LICENSE
//
// ----------------------------------------------------------------------------

#define _USE_MATH_DEFINES
#include <cmath>
#include <vector>

#include "scenepic.h"

namespace sp = scenepic;

typedef std::shared_ptr<sp::Canvas3D> CanvasPtr;


void set_audio(sp::Scene &scene, CanvasPtr &canvas, const std::string &path)
{
    auto audio = scene.create_audio();
    audio->load(path);
    canvas->media_id(audio->audio_id());
}


int main(int argc, char *argv[])
{
    sp::Scene scene;

    std::vector<std::string> names = {"red", "green", "blue"};
    std::vector<sp::Color> colors = {sp::Colors::Red, sp::Colors::Green, sp::Colors::Blue};
    std::vector<float> frequencies = {0, 1, 0.5};

    auto graph = scene.create_graph("graph", 600, 150, "graph");
    for(int i=0; i<3; ++i)
    {
        auto mesh = scene.create_mesh();
        mesh->add_cube(colors[i]);
        auto canvas = scene.create_canvas_3d(names[i], 200, 200, names[i]);
        set_audio(scene, canvas, names[i] + ".ogg");
        std::vector<float> values;

        for(int j=0; j<60; ++j)
        {
            auto frame = canvas->create_frame();
            double scale = std::sin(j * 2 * M_PI * frequencies[i] / 30);
            values.push_back(static_cast<float>(scale));
            frame->add_mesh(mesh, sp::Transforms::scale(static_cast<float>((scale + 1) / 2 + 0.5)));
        }

        graph->add_sparkline(names[i], values, colors[i]);
        graph->media_id(canvas->media_id());
    }
    
    names.push_back("graph");
    scene.link_canvas_events(names);
    scene.grid("600px", "1fr auto", "1fr 1fr 1fr");
    scene.place("graph", "2", "1 / span 3");
    scene.save_as_html("audio_tracks.html", "Audio Tracks");
}