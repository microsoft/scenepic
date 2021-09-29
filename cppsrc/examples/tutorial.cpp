// ----------------------------------------------------------------------------
//
// tutorial.cpp -- ScenePic C++ tutorial
//
// Copyright (C) 2021 Microsoft
//
// For conditions of distribution and use, see copyright notice in LICENSE
//
// ----------------------------------------------------------------------------

#include <iostream>

#include "scenepic.h"

namespace sp = scenepic;

void scene_and_canvas_basics()
{
    std::cout << "== Scene and Canvas Basics ==" << std::endl;
    sp::Scene scene;
    scene.create_canvas_3d("300", 300, 300);
    scene.create_canvas_3d("100", 100, 300);
    scene.save_as_html("scene_and_canvas_basics.html");
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

    scene.save_as_html("meshes_and_frames.html");
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

    scene.save_as_html("point_clouds_1.html");
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

    scene.save_as_html("point_clouds_2.html");
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

    canvas1->set_layer_settings({
        {"Coords", sp::LayerSettings().filled(false)},
        {"Sphere+", sp::LayerSettings().filled(true)}
    });

    canvas1->create_frame("", sp::FocusPoint::None(), {mesh1->mesh_id(), mesh2->mesh_id()});
    canvas2->create_frame("", sp::FocusPoint::None(), {mesh2->mesh_id(), mesh3->mesh_id()});
    canvas2->create_frame("", sp::FocusPoint::None(), {mesh4->mesh_id(), mesh1->mesh_id()});

    scene.save_as_html("misc_meshes.html");
}

int main(int argc, char *argv[])
{
    scene_and_canvas_basics();
    meshes_and_frames();
    point_clouds_1();
    point_clouds_2();
    misc_meshes();
}