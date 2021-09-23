// ----------------------------------------------------------------------------
//
// circles_video.cpp -- ScenePic video integration example
//
// Copyright (C) 2019 Microsoft
//
// For conditions of distribution and use, see copyright notice in LICENSE
//
// ----------------------------------------------------------------------------

#define _USE_MATH_DEFINES
#include <cmath>
#include <vector>
#include <utility>

#include "scenepic.h"


namespace sp = scenepic;

const int SIZE = 400;


std::pair<float, float> angle_to_pos(float angle, float radius)
{
    int x = static_cast<int>(std::cos(angle) * radius) + SIZE / 2;
    int y = static_cast<int>(std::sin(angle) * radius) + SIZE / 2;
    return std::make_pair(static_cast<float>(x), static_cast<float>(y));
}

int main(int argc, char *argv[])
{
    sp::Scene scene;

    auto video = scene.create_video();
    video->load("circles.mp4");

    auto tracking = scene.create_canvas_2d("tracking", SIZE, SIZE);
    tracking->background_color(sp::Colors::White);
    tracking->media_id(video->video_id());
    auto multi = scene.create_canvas_2d("multi", SIZE, SIZE);
    multi->background_color(sp::Colors::White);
    multi->media_id(video->video_id());

    for(int i=0; i<360; ++i){
        auto frame = tracking->create_frame();
        frame->add_video("fit", 0.0f, 0.0f, 1.0f, false);

        float angle = static_cast<float>(i * M_PI / 180.0f);
        auto red_pos = angle_to_pos(angle, 160);
        frame->add_rectangle(red_pos.first - 11, red_pos.second - 11, 22, 22, sp::Color::from_bytes(255, 0, 0), 2, sp::Color::None(), "rect");
        frame->add_circle(red_pos.first, red_pos.second, 10, sp::Color::from_bytes(255, 0, 0), 1.0f, sp::Color::from_bytes(255, 0, 0), "dot");

        auto green_pos = angle_to_pos(-2*angle, 80);
        frame->add_rectangle(green_pos.first - 11, green_pos.second - 11, 22, 22, sp::Color::from_bytes(0, 255, 0), 2, sp::Color::None(), "rect");
        frame->add_circle(green_pos.first, green_pos.second, 10, sp::Color::from_bytes(0, 255, 0), 1.0f, sp::Color::from_bytes(0, 255, 0), "dot");

        auto blue_pos = angle_to_pos(4*angle, 40);
        frame->add_rectangle(blue_pos.first - 11, blue_pos.second - 11, 22, 22, sp::Color::from_bytes(0, 0, 255), 2, sp::Color::None(), "rect");
        frame->add_circle(blue_pos.first, blue_pos.second, 10, sp::Color::from_bytes(0, 0, 255), 1.0f, sp::Color::from_bytes(0, 0, 255), "dot");

        frame = multi->create_frame();
        frame->add_video("manual", red_pos.first - 40, red_pos.second - 40, 0.2f, false, "red");
        frame->add_video("manual", green_pos.first - 25, green_pos.second - 25, 0.125f, false, "green");
        frame->add_video("manual", 160, 160, 0.2f, false, "blue");
    }

    tracking->set_layer_settings({
        {"rect", sp::LayerSettings().render_order(0)},
        {"dot", sp::LayerSettings().render_order(1)}
    });

    scene.link_canvas_events({"tracking", "multi"});
    scene.save_as_html("circles_video.html", "Video Example");
}