// ----------------------------------------------------------------------------
//
// mesh_animation.cpp -- ScenePic mesh animation example app
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

#include "svt.h"


int main(int argc, char *argv[])
{
    svt::Scene scene;
    auto canvas = scene.create_canvas_3d("jelly", 700, 700);

    auto jelly_mesh = svt::load_obj("jelly.obj");
    auto texture = scene.create_image("texture");
    texture->load("jelly.png");

    auto base_mesh = scene.create_mesh("jelly_base");
    base_mesh->texture_id(texture->image_id()).use_texture_alpha(true);
    base_mesh->add_mesh(jelly_mesh);

    auto marble = scene.create_mesh("marble");
    marble->shared_color(svt::Colors::White);
    marble->add_sphere(svt::Color::None(), svt::Transforms::scale(0.4f));

    for(auto i=0; i<60; ++i)
    {
        auto frame = canvas->create_frame();
        svt::VectorBuffer positions = jelly_mesh->position_buffer();
        auto delta_x = (positions.col(0).array() + 0.0838f * i) * 10.0f;
        auto delta_z = (positions.col(2).array() + 0.0419f * i) * 10.0f;
        positions.col(1) = positions.col(1).array() + 0.1 * (delta_x.cos() + delta_z.sin());
        
        auto mesh_update = scene.update_mesh("jelly_base", positions);
        frame->add_mesh(mesh_update);

        float marble_y = std::sin(0.105f * i);
        frame->add_mesh(marble, svt::Transforms::translate(svt::Vector(0, marble_y, 0)));
    }

    std::cout << std::endl << "Before compression:" << std::endl;
    auto command_sizes = scene.measure_command_size();
    for(auto& item : command_sizes)
    {
        std::cout << item.first << ": " << item.second << std::endl;
    }

    auto info = scene.quantize_updates();
    std::cout << info["jelly_base"].to_string() << std::endl;
    std::cout << std::endl << "compressing updates..." << std::endl;

    command_sizes = scene.measure_command_size();
    std::cout << std::endl << "After compression:" << std::endl;
    for(auto& item : command_sizes)
    {
        std::cout << item.first << ": " << item.second << std::endl;
    }

    scene.save_as_html("mesh_animation.html", "Mesh Animation");
}
