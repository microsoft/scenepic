// ----------------------------------------------------------------------------
//
// hello_world.cpp -- ScenePic Hello World example app
//
// Copyright (C) 2019 Microsoft
//
// For conditions of distribution and use, see copyright notice in LICENSE
//
// ----------------------------------------------------------------------------

#define _USE_MATH_DEFINES
#include <cmath>

#include <string>
#include <fstream>

#include "svt.h"

std::shared_ptr<svt::Mesh> create_image_mesh(svt::Scene &scene, const std::string &path, const std::string &layer_id)
{
    auto image = scene.create_image();
    image->load(path);
    auto mesh = scene.create_mesh();
    mesh->texture_id(image->image_id())
        .layer_id(layer_id)
        .nn_texture(false);
    mesh->add_image(svt::Vector(-0.5f, -0.5f, 0), svt::Vector(2, 0, 0));
    return mesh;
}

int main(int argc, char *argv[])
{
    svt::Scene scene;
    auto mesh_hello = create_image_mesh(scene, "Hello.png", "Hello");
    auto mesh_analog = create_image_mesh(scene, "Analog.png", "Analog Science");
    auto mesh_science = create_image_mesh(scene, "Science.png", "Analog Science");
    // Note re-use of layer_id to group with mesh_analog

    auto audio_hello = scene.create_audio();
    audio_hello->load("hello.mp3");

    auto hand_mesh = svt::load_obj("hand.obj");
    auto hand_mesh_hi = hand_mesh->subdivide(1, true);

    auto model_mesh_lo = scene.create_mesh();
    model_mesh_lo->shared_color(svt::Color(1, 0, 0));
    model_mesh_lo->add_mesh(hand_mesh);
    model_mesh_lo->reverse_triangle_order();

    auto model_mesh_hi = scene.create_mesh();
    model_mesh_hi->shared_color(svt::Color(0, 0, 1));
    model_mesh_hi->add_mesh(hand_mesh_hi);
    model_mesh_hi->reverse_triangle_order();

    const std::size_t Size = 500;

    auto canvas_hello = scene.create_canvas_3d("", Size, Size);
    canvas_hello->media_id(audio_hello->audio_id());
    svt::Vector hand_center = hand_mesh->position_buffer().colwise().mean();
    svt::Camera hand_camera(hand_center + svt::Vector(0, 0, 0.5), hand_center, svt::Vector(0, 1, 0), 45.0);
    svt::Shading hand_shading(svt::Colors::White);
    svt::UIParameters hand_ui_parameters;
    auto canvas_hand = scene.create_canvas_3d("", Size, Size);
    canvas_hand->camera(hand_camera)
        .shading(hand_shading)
        .ui_parameters(hand_ui_parameters);

    const std::size_t n_frames = 30;
    const float PI = static_cast<float>(M_PI);
    for (std::size_t i = 0; i < n_frames; ++i)
    {
        float angle = static_cast<float>(2 * PI * i / n_frames);

        // Add some meshes with transforms
        auto frame_hello = canvas_hello->create_frame();
        frame_hello->add_mesh(mesh_hello, svt::Transforms::translate({std::cos(angle), std::sin(angle), 0}));
        frame_hello->add_mesh(mesh_analog, svt::Transforms::translate({std::cos(angle - 0.5f * PI), std::sin(angle - 0.5f * PI), -0.5f}));
        frame_hello->add_mesh(mesh_science, svt::Transforms::translate({std::cos(angle + PI), std::sin(angle + PI), -1.0f}));

        // Add some further primitives randomized per frame
        auto mesh_primitives = scene.create_mesh();
        mesh_primitives->layer_id("Primitives");
        mesh_primitives->add_sphere(svt::Color(0, 1, 0), svt::Transforms::scale(0.2f + 0.2f * (1 + std::cos(angle))), false, true);
        mesh_primitives->add_cube(svt::Color(0, 0, 1), svt::Transforms::translate({-1.0f, -1.0f, -3.0f}));
        frame_hello->add_mesh(mesh_primitives);

        // Add a noise pointcloud
        auto mesh_noise = scene.create_mesh();
        mesh_noise->shared_color(svt::Color(1, 0, 0)).layer_id("Noise");
        // The primitive for the pointcloud
        mesh_noise->add_cylinder();
        // Squash this to the right size
        mesh_noise->apply_transform(svt::Transforms::scale({0.02f, 0.1f, 0.1f}));
        // Rotate (arbitrary example)
        mesh_noise->apply_transform(svt::Transforms::rotation_to_align_x_to_axis({0.5f, 0.5f, 0.5f}));
        // Add random instances of the primitive.  Each instance can also optionally have its own color and quaternion rotation.
        mesh_noise->enable_instancing(svt::VectorBuffer::Random(1000, 3));
        frame_hello->add_mesh(mesh_noise);

        auto frame_hand = canvas_hand->create_frame("", hand_center);
        frame_hand->add_mesh(model_mesh_lo, svt::Transforms::rotation_about_z(std::cos(angle)));
        frame_hand->add_mesh(model_mesh_hi, svt::Transforms::rotation_about_z(std::cos(angle + PI)));
    }

    canvas_hello->set_layer_settings({{"Primitives", svt::LayerSettings().filled(false)},
                                      {"Noise", svt::LayerSettings().filled(false).opacity(0.5f)}});


    scene.save_as_html("HelloWorld.html", "Hello World ScenePic!");

    return 0;
}