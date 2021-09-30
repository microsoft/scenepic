// ----------------------------------------------------------------------------
//
// tutorial.cpp -- ScenePic C++ tutorial
//
// Copyright (C) 2021 Microsoft
//
// For conditions of distribution and use, see copyright notice in LICENSE
//
// ----------------------------------------------------------------------------

#define _USE_MATH_DEFINES
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <map>
#include <string>
#include <sstream>

#include "scenepic.h"

namespace sp = scenepic;

int randint(int max)
{
    return std::rand() % max;
}

float randf(float min = 0.0f, float max = 1.0f)
{
    float range = max - min;
    return (range * std::rand()) / static_cast<float>(RAND_MAX) + min;
}

void scene_and_canvas_basics()
{
    std::cout << "== Scene and Canvas Basics ==" << std::endl;
    sp::Scene scene;
    scene.create_canvas_3d("300", 300, 300);
    scene.create_canvas_3d("100", 100, 300);
    scene.save_as_html("scene_and_canvas_basics.html", "Scene and Canvas Basics");
}

void meshes_and_frames()
{
    std::cout << "== Meshes and Frames ==" << std::endl;
    sp::Scene scene;

    auto mesh = scene.create_mesh();
    mesh->shared_color(sp::Color(1, 0, 1));
    mesh->add_cube(sp::Color::None(), sp::Transforms::scale(0.1f));
    mesh->add_cube(sp::Color::None(), sp::Transforms::translate({-1, 1, -1}) * sp::Transforms::scale(0.5f));
    mesh->add_sphere(sp::Color::None(), sp::Transforms::translate({1, 1, 1}));

    auto canvas = scene.create_canvas_3d("", 300, 300);

    for (int i = 0; i < 10; ++i)
    {
        auto frame = canvas->create_frame();
        frame->add_mesh(mesh, sp::Transforms::translate({i / 10.0f, 0, 0}));
        auto mesh2 = scene.create_mesh();
        mesh2->shared_color(sp::Color(1, 0, 0))
            .camera_space(true);
        mesh2->add_cube(sp::Color::None(), sp::Transforms::translate({0, 0, -5}) * sp::Transforms::scale(0.5f));
        frame->add_mesh(mesh2);
        auto label = scene.create_label("", "Hi", sp::Colors::White, 80);
        label->offset_distance(0.6f)
            .camera_space(true);
        frame->add_label(label, {0, 0, -5});
    }

    scene.save_as_html("meshes_and_frames.html", "Meshes and Frames");
}

void point_clouds_1()
{
    std::cout << "== Point Clouds 1 ==" << std::endl;
    sp::Scene scene;

    auto mesh = scene.create_mesh();
    mesh->shared_color(sp::Color(0, 1, 0));
    mesh->add_cube();
    mesh->apply_transform(sp::Transforms::scale(0.01f));
    sp::VectorBuffer positions = sp::random<sp::VectorBuffer>(10000, 3);
    positions = (2 * positions).array() - 1;
    mesh->enable_instancing(positions);

    auto canvas = scene.create_canvas_3d("", 300, 300);
    canvas->shading(sp::Shading(sp::Colors::White));
    auto frame = canvas->create_frame();
    frame->add_mesh(mesh);

    scene.save_as_html("point_clouds_1.html", "Point Clouds 1");
}

void point_clouds_2()
{
    std::cout << "== Point Clouds 2 ==" << std::endl;

    float disc_thickness = 0.2f;
    float normal_length = 1.5f;
    float point_size = 0.1f;

    sp::Mesh sphere_mesh;
    sphere_mesh.add_sphere(sp::Color(1, 0, 0), sp::Transforms::scale(2));
    auto num_vertices = sphere_mesh.count_vertices();
    auto points = sphere_mesh.vertex_positions();
    auto normals = sphere_mesh.vertex_normals();

    sp::QuaternionBuffer rotations = sp::QuaternionBuffer::Zero(num_vertices, 4);
    for (int i = 0; i < num_vertices; ++i)
    {
        rotations.row(i) = sp::Transforms::quaternion_to_align_x_to_axis(normals.row(i));
    }

    sp::ColorBuffer colors = sp::random<sp::ColorBuffer>(num_vertices, 3);

    sp::Scene scene;

    auto mesh = scene.create_mesh();
    mesh->shared_color(sp::Color(0, 1, 0))
        .double_sided(true);

    mesh->add_disc(sp::Color::None(), sp::Transforms::scale({disc_thickness, 1, 1}), 20);
    mesh->add_thickline(sp::Color::None(), {disc_thickness * 0.5f, 0, 0}, {normal_length, 0, 0}, 0.2f, 0.1f);
    mesh->apply_transform(sp::Transforms::scale(point_size));

    mesh->enable_instancing(points, rotations, colors);

    auto canvas = scene.create_canvas_3d("", 300, 300);
    auto frame = canvas->create_frame();
    frame->add_mesh(mesh);

    scene.save_as_html("point_clouds_2.html", "Point Clouds 2");
}

void misc_meshes()
{
    std::cout << "== Miscellaneous Meshes ==" << std::endl;
    sp::Scene scene;

    auto mesh1 = scene.create_mesh("sphere+", "Sphere+");
    mesh1->add_cube(sp::Color(1, 0, 0), sp::Transforms::translate({-2, 0, -2}));
    mesh1->add_uv_sphere(sp::Color(0, 0, 1), sp::Transforms::translate({-1, 1, 0}) * sp::Transforms::scale(1.8f), 10, 10, false, true);
    mesh1->add_icosphere(sp::Color(0, 1, 1), sp::Transforms::translate({2, 1, 0}) * sp::Transforms::scale(1.8f), 2, false, true);

    auto mesh2 = scene.create_mesh("coords", "Coords");
    mesh2->add_coordinate_axes();

    sp::VectorBuffer cube_verts(8, 3);
    cube_verts << -0.5, -0.5, -0.5,
        +0.5, -0.5, -0.5,
        -0.5, +0.5, -0.5,
        +0.5, +0.5, -0.5,
        -0.5, -0.5, +0.5,
        +0.5, -0.5, +0.5,
        -0.5, +0.5, +0.5,
        +0.5, +0.5, +0.5;

    sp::TriangleBuffer cube_tris(12, 3);
    cube_tris << 0, 2, 3,
        0, 3, 1,
        1, 3, 7,
        1, 7, 5,
        4, 5, 7,
        4, 7, 6,
        4, 6, 2,
        4, 2, 0,
        2, 6, 7,
        2, 7, 3,
        4, 0, 1,
        4, 1, 5;
    sp::LoopSubdivisionStencil stencil = sp::LoopSubdivisionStencil::create(cube_tris, 2, false);
    sp::VectorBuffer cube_verts_a = stencil.apply(cube_verts);
    sp::TriangleBuffer cube_tris_a = stencil.triangles();

    stencil = sp::LoopSubdivisionStencil::create(cube_tris, 2, true);
    sp::VectorBuffer cube_verts_b = stencil.apply(cube_verts);
    sp::TriangleBuffer cube_tris_b = stencil.triangles();

    auto mesh3 = scene.create_mesh();
    mesh3->shared_color(sp::Color(1, 0.8f, 0.8f));
    mesh3->add_mesh_without_normals(cube_verts, cube_tris, sp::ColorBufferNone(), sp::UVBufferNone(),
                                    sp::Transforms::translate({-1, 0, 0}));
    mesh3->add_mesh_without_normals(cube_verts_a, cube_tris_a);
    mesh3->add_mesh_without_normals(cube_verts_b, cube_tris_b, sp::ColorBufferNone(), sp::UVBufferNone(),
                                    sp::Transforms::translate({1, 0, 0}));

    auto mesh4 = scene.create_mesh();
    int num_segs = 7000;

    sp::VertexBuffer colored_points(num_segs, 6);
    colored_points.leftCols(3) = sp::rowwise_cumsum(sp::random<sp::VectorBuffer>(num_segs, 3) * 0.2);
    colored_points.rightCols(3) = sp::random<sp::ColorBuffer>(num_segs, 3);

    mesh4->add_lines(colored_points.topRows(num_segs - 1), colored_points.bottomRows(num_segs - 1));
    mesh4->add_camera_frustum(sp::Color(1, 1, 0));

    auto canvas1 = scene.create_canvas_3d("canvas1", 300, 300);
    auto canvas2 = scene.create_canvas_3d("canvas2", 300, 300);

    scene.link_canvas_events({"canvas1", "canvas2"});

    canvas1->set_layer_settings({{"Coords", sp::LayerSettings().filled(false)},
                                 {"Sphere+", sp::LayerSettings().filled(true)}});

    canvas1->create_frame("", sp::FocusPoint::None(), {mesh1->mesh_id(), mesh2->mesh_id()});
    canvas2->create_frame("", sp::FocusPoint::None(), {mesh2->mesh_id(), mesh3->mesh_id()});
    canvas2->create_frame("", sp::FocusPoint::None(), {mesh4->mesh_id(), mesh1->mesh_id()});

    scene.save_as_html("misc_meshes.html", "Miscellaneous Meshes");
}

void images_and_textures()
{
    std::cout << "== Images and Textures ==" << std::endl;
    sp::Scene scene;

    auto image1 = scene.create_image("PolarBear");
    image1->load("PolarBear.png");

    auto texture = scene.create_image("texture");
    texture->load("uv.png");

    auto cam_space_mesh = scene.create_mesh();
    cam_space_mesh->shared_color(sp::Color(1, 0, 0));
    cam_space_mesh->camera_space(true);
    cam_space_mesh->add_sphere(sp::Color::None(), sp::Transforms::translate({10, -10, -20}));

    auto sphere = scene.create_mesh();
    sphere->texture_id(texture->image_id());
    sphere->nn_texture(false);
    sphere->add_icosphere(sp::Color::None(), sp::Transform::Identity(), 4);

    auto cube = scene.create_mesh();
    cube->texture_id(texture->image_id());
    auto transform = sp::Transforms::translate({-1, 0, 0}) * sp::Transforms::scale(0.5f);
    cube->add_cube(sp::Color::None(), transform);

    auto canvas = scene.create_canvas_3d();
    canvas->shading(sp::Shading(sp::Colors::White));
    auto mesh1 = scene.create_mesh();
    mesh1->texture_id("PolarBear");
    mesh1->add_image();

    int n_frames = 20;
    for (int i = 0; i < n_frames; ++i)
    {
        float angle = static_cast<float>(2 * M_PI * i / n_frames);
        float cos = std::cosf(angle);
        float sin = std::sinf(angle);

        sp::Vector axis(1, 0, 1);
        axis.normalize();
        sp::FocusPoint focus_point({cos, sin, 0}, axis * angle);

        auto mesh = scene.create_mesh();
        mesh->add_coordinate_axes(1, 0.1f, sp::Transforms::translate(focus_point.position()) * sp::Transforms::rotation_matrix_from_axis_angle(axis, angle));

        auto image_b = scene.create_image();
        image_b->load("rand15x15.png");
        auto mesh_b = scene.create_mesh();
        mesh_b->texture_id(image_b->image_id());
        mesh_b->is_billboard(true);
        mesh_b->use_texture_alpha(true);
        mesh_b->add_image();
        mesh_b->apply_transform(sp::Transforms::scale(2.0) * sp::Transforms::translate({0, 0, -1}));

        auto frame = canvas->create_frame();
        frame->focus_point(focus_point);
        frame->add_mesh(mesh1, sp::Transforms::translate({cos, sin, 0}));
        frame->add_mesh(mesh_b, sp::Transforms::scale(i * 1.0 / n_frames) * sp::Transforms::translate({-cos, -sin, 0}));
        frame->add_mesh(cam_space_mesh);
        frame->add_mesh(sphere, sp::Transforms::rotation_about_y(static_cast<float>(M_PI * 2 * i / n_frames)));
        frame->add_mesh(cube, sp::Transforms::rotation_about_y(static_cast<float>(-M_PI * 2 * i / n_frames)));
        frame->add_mesh(mesh);
    }

    scene.save_as_html("images_and_textures.html", "Images and Textures");
}

void canvas_2d()
{
    std::cout << "== 2D Canvases ==" << std::endl;
    sp::Scene scene;

    auto image1 = scene.create_image("PolarBear");
    image1->load("PolarBear.png");

    auto image2 = scene.create_image("Random");
    image2->load("rand30x20.png");

    auto canvas1 = scene.create_canvas_2d("", 400, 300);
    canvas1->background_color(sp::Colors::White);
    canvas1->create_frame()->add_image(image1->image_id(), "fit");
    canvas1->create_frame()->add_image(image1->image_id(), "fill");
    canvas1->create_frame()->add_image(image1->image_id(), "stretch");
    canvas1->create_frame()->add_image(image1->image_id(), "manual", 50, 50, 0.3f);

    auto canvas2 = scene.create_canvas_2d("", 300, 300);
    auto frame = canvas2->create_frame();
    frame->add_image(image2->image_id(), "fit");
    frame->add_image(image1->image_id(), "manual", 30, 30, 0.2f);
    frame->add_circle(200, 200, 40, sp::Colors::Blue, 10, sp::Colors::Black);
    frame->add_rectangle(200, 100, 50, 25, sp::Colors::Black, 0, sp::Colors::Green);
    frame->add_text("Hello World", 30, 100, sp::Colors::White, 100, "segoe ui light");

    scene.save_as_html("canvas_2d.html", "2D Canvases");
}

void opacity_and_labels()
{
    std::cout << "== Opacity and Labels ==" << std::endl;

    std::srand(55);

    sp::Scene scene;
    auto canvas = scene.create_canvas_3d("", 700, 700);
    auto frame = canvas->create_frame();

    std::string horizontal_align_options[] = {"left", "center", "right"};
    std::string vertical_align_options[] = {"top", "middle", "bottom"};

    std::map<std::string, sp::LayerSettings> layer_settings;
    layer_settings["Labels"] = sp::LayerSettings();
    int num_objects = 20;
    for (int i = 0; i < num_objects; ++i)
    {
        int geotype = randint(2);
        sp::Color color = sp::random<sp::Color>();
        float size = randf(0.2f, 0.5f);
        sp::Vector position = sp::random<sp::Vector>(-1.5f, 1.5f);
        float opacity = randint(2) == 0 ? 1.0f : randf(0.45, 0.55);

        std::string layer_id = "Layer" + std::to_string(i);
        auto mesh = scene.create_mesh("", layer_id);
        mesh->shared_color(color);
        layer_settings[layer_id] = sp::LayerSettings();
        if (geotype)
        {
            mesh->add_sphere();
        }
        else
        {
            mesh->add_cube();
        }

        mesh->apply_transform(sp::Transforms::scale(size));
        mesh->apply_transform(sp::Transforms::translate(position));
        frame->add_mesh(mesh);

        std::stringstream text;
        text.precision(2);
        text << color[0] << " " << color[1] << " " << color[2] << " " << opacity;
        std::string horizontal_align = horizontal_align_options[randint(3)];
        std::string vertical_align = vertical_align_options[randint(3)];

        float offset_distance;
        if (geotype)
        {
            if (horizontal_align != "center" && vertical_align != "middle")
            {
                offset_distance = size * 0.5f * 0.8f;
            }
            else
            {
                offset_distance = size * 0.6f;
            }
        }
        else
        {
            if (horizontal_align != "center" && vertical_align != "middle")
            {
                offset_distance = size * 0.7;
            }
            else
            {
                offset_distance = size * 0.9;
            }
        }

        auto label = scene.create_label("", text.str(), color, 80 * size, "consolas", "Labels", horizontal_align, vertical_align, offset_distance);
        frame->add_label(label, position);
    }

    canvas->set_layer_settings(layer_settings);

    scene.save_as_html("opacity_and_labels.html", "Opacity and Labels");
}

void animation()
{
    std::cout << "== Animation ==" << std::endl;

    sp::Scene scene;
    auto canvas = scene.create_canvas_3d("jelly", 700, 700);

    auto jelly_mesh = sp::load_obj("jelly.obj");
    auto texture = scene.create_image("texture");
    texture->load("jelly.png");

    auto base_mesh = scene.create_mesh("jelly_base");
    base_mesh->texture_id(texture->image_id()).use_texture_alpha(true);
    base_mesh->add_mesh(jelly_mesh);

    auto marble = scene.create_mesh("marble");
    marble->shared_color(sp::Colors::White);
    marble->add_sphere(sp::Color::None(), sp::Transforms::scale(0.4f));

    for (auto i = 0; i < 60; ++i)
    {
        auto frame = canvas->create_frame();
        sp::VectorBuffer positions = jelly_mesh->position_buffer();
        auto delta_x = (positions.col(0).array() + 0.0838f * i) * 10.0f;
        auto delta_z = (positions.col(2).array() + 0.0419f * i) * 10.0f;
        positions.col(1) = positions.col(1).array() + 0.1 * (delta_x.cos() + delta_z.sin());

        auto mesh_update = scene.update_mesh_without_normals("jelly_base", positions);
        frame->add_mesh(mesh_update);

        float marble_y = std::sin(0.105f * i);
        frame->add_mesh(marble, sp::Transforms::translate(sp::Vector(0, marble_y, 0)));
    }

    std::cout << std::endl
              << "Before compression:" << std::endl;
    auto command_sizes = scene.measure_command_size();
    for (auto &item : command_sizes)
    {
        std::cout << item.first << ": " << item.second << std::endl;
    }

    auto info = scene.quantize_updates();
    std::cout << info["jelly_base"].to_string() << std::endl;
    std::cout << std::endl
              << "compressing updates..." << std::endl;

    command_sizes = scene.measure_command_size();
    std::cout << std::endl
              << "After compression:" << std::endl;
    for (auto &item : command_sizes)
    {
        std::cout << item.first << ": " << item.second << std::endl;
    }

    scene.save_as_html("animation.html", "Animation");
}

int main(int argc, char *argv[])
{
    scene_and_canvas_basics();
    meshes_and_frames();
    point_clouds_1();
    point_clouds_2();
    misc_meshes();
    images_and_textures();
    canvas_2d();
    opacity_and_labels();
    animation();
}