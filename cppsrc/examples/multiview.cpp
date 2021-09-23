// ----------------------------------------------------------------------------
//
// multiview.cpp -- ScenePic mesh animation example app
//
// Copyright (C) 2021 Microsoft
//
// For conditions of distribution and use, see copyright notice in LICENSE
//
// ----------------------------------------------------------------------------

#define _USE_MATH_DEFINES
#include <cmath>

#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <string>
#include <vector>

#include <Eigen/Core>
#include <Eigen/Dense>

#include "svt.h"


svt::Camera load_camera(const svt::JsonValue& camera_info)
{
    svt::Vector location;
    location.x() = camera_info["location"].values()[0].as_float();
    location.y() = camera_info["location"].values()[1].as_float();
    location.z() = camera_info["location"].values()[2].as_float();

    svt::Vector euler_angles;
    euler_angles.x() = camera_info["rotation"].values()[0].as_float();
    euler_angles.y() = camera_info["rotation"].values()[1].as_float();
    euler_angles.z() = camera_info["rotation"].values()[2].as_float();

    float fov = camera_info["fov"].as_float();
    float aspect_ratio = camera_info["width"].as_float() / camera_info["height"].as_float();

    auto rotation = svt::Transforms::euler_angles_to_matrix(euler_angles, "XYZ");
    auto translation = svt::Transforms::translate(location);
    auto extrinsics = (translation * rotation).eval();
    auto world_to_camera = svt::Transforms::gl_world_to_camera(extrinsics);

    auto projection = svt::Transforms::gl_projection(fov, aspect_ratio, 0.01, 100);
    return svt::Camera(world_to_camera, projection);
}


std::vector<svt::Camera> load_cameras()
{
    std::ifstream input("cameras.json");
    auto cameras = svt::JsonValue::parse(input);
    return {
        load_camera(cameras["cam0"]),
        load_camera(cameras["cam1"]),
        load_camera(cameras["cam2"])
    };
}


int main(int argc, char *argv[])
{
    svt::Scene scene;

    auto cameras = load_cameras();

    auto texture = scene.create_image("texture");
    texture->load("PolarBear.png");

    auto cube = scene.create_mesh("cube");
    cube->texture_id(texture->image_id());
    cube->add_cube(svt::Color::None(), svt::Transforms::scale(2));

    auto frustums = scene.create_mesh("frustums", "frustums");
    std::vector<svt::Color> colors = {svt::Colors::Red, svt::Colors::Green, svt::Colors::Blue};
    std::vector<std::string> paths = {"render0.png", "render1.png", "render2.png"};
    std::vector<std::string> camera_images;
    std::vector<std::string> images;

    for(int i=0; i<3; ++i)
    {
        auto image = scene.create_image(paths[i]);
        image->load(paths[i]);
        frustums->add_camera_frustum(cameras[i], colors[i]);
        auto image_mesh = scene.create_mesh("image" + std::to_string(i), "images");
        image_mesh->texture_id(image->image_id()).shared_color(svt::Colors::Gray).double_sided(true);
        image_mesh->add_camera_image(cameras[i]);

        images.push_back(image->image_id());
        camera_images.push_back(image_mesh->mesh_id());
    }

    float width = 640;
    for(int i=0; i<3; ++i)
    {
        float height = width / cameras[i].aspect_ratio();
        auto canvas = scene.create_canvas_3d("hand" + std::to_string(i), width, height, "", cameras[i]);

        auto frame = canvas->create_frame();
        frame->add_mesh(cube);
        frame->add_mesh(frustums);
        frame->camera(cameras[i]);
        for(int j=0; j<3; ++j)
        {
            frame->add_mesh_by_id(camera_images[j]);
        }
    }

    scene.save_as_html("multiview.html", "Multiview");
}
