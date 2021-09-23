// ----------------------------------------------------------------------------
//
// waves.cpp -- ScenePic line drawing example app
//
// Copyright (C) 2019 Microsoft
//
// For conditions of distribution and use, see copyright notice in LICENSE
//
// ----------------------------------------------------------------------------

#define _USE_MATH_DEFINES
#include <cmath>

#include <iostream>
#include <algorithm>
#include <string>
#include <vector>

#include <Eigen/Core>

#include "scenepic.h"


namespace sp = scenepic;


sp::Color to_color(double hue)
{
    float hh = static_cast<float>(hue * 3 / M_PI);
    int i = static_cast<int>(hh);
    float ff = hh - i;
    float p = 0;
    float q = 1.0f - ff;
    float t = 1.0f - (1.0f - ff);

    sp::Color color;

    switch (i)
    {
    case 0:
        color.r() = 1;
        color.g() = t;
        color.b() = p;
        break;
    case 1:
        color.r() = q;
        color.g() = 1;
        color.b() = p;
        break;
    case 2:
        color.r() = p;
        color.g() = 1;
        color.b() = t;
        break;

    case 3:
        color.r() = p;
        color.g() = q;
        color.b() = 1;
        break;
    case 4:
        color.r() = t;
        color.g() = p;
        color.b() = 1;
        break;
    case 5:
    default:
        color.r() = 1;
        color.g() = p;
        color.b() = q;
        break;
    }

    return color;
}

int main(int argc, char *argv[])
{
    sp::Scene scene;

    float width = 600;
    float height = 300;
    auto canvas = scene.create_canvas_2d("waves", width, height, "waves", sp::Colors::White);

    std::size_t num_frames = 120;
    std::size_t num_points = 20;
    for (auto i = 0; i < num_frames; ++i)
    {
        auto frame = canvas->create_frame();
        float offset = static_cast<float>(M_PI * 2 * i / num_frames);
        sp::Color color = to_color(offset);
        float diff = static_cast<float>(M_PI * 2 / num_points);
        sp::CoordinateBuffer coords(num_points, 2);
        for (auto j = 0; j < num_points; ++j)
        {
            float angle = offset + diff * j;
            float x = (j * width) / (num_points - 1);
            float y = std::sin(angle) * 100 + height / 2;
            coords.row(j) << x, y;
            frame->add_circle(x, y, 4, color, 1, color);
        }

        frame->add_line(coords, color, 2);
    }

    scene.save_as_html("waves.html");
}