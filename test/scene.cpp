#define _USE_MATH_DEFINES
#include <cmath>

#include <utility>

#include "scene.h"
#include "transforms.h"
#include "svt_tests.h"


namespace {
std::pair<svt::VectorBuffer, svt::TriangleBuffer> create_tetrahedron()
{
    svt::VectorBuffer vertices(4, 3);
    vertices << -0.5f, -0.32476f, -0.20412f,
                0.5f, -0.32476f, -0.20412f,
                0, 0.541266f, -0.20412f,
                0, 0.108253f, 0.612372f;

    svt::TriangleBuffer triangles(4, 3);
    triangles <<  0, 1, 3,
                  1, 2, 3,
                  2, 0, 3,
                  0, 2, 1;
    
    return {vertices, triangles};
}

const std::size_t SIZE = 500;

const float PI = static_cast<float>(M_PI);
}

int test_scene()
{
    int result = EXIT_SUCCESS;

    svt::Scene scene("test");
    auto image = scene.create_image("rand");
    image->load(test::asset_path("rand.png"));
    auto mesh_rand = scene.create_mesh();
    mesh_rand->texture_id(image->image_id())
              .layer_id("Test");
    mesh_rand->add_image(svt::Vector(-0.5f, -0.5f, 0), svt::Vector(2, 0, 0));

    auto tet = create_tetrahedron();

    auto model_mesh = scene.create_mesh();
    model_mesh->shared_color(svt::Color(1, 0, 0));
    model_mesh->add_mesh_without_normals(tet.first, tet.second);
    model_mesh->reverse_triangle_order();

    auto canvas_rand = scene.create_canvas_3d("", SIZE, SIZE);
    svt::Vector tet_center = tet.first.colwise().mean();
    auto tet_camera = svt::Camera(tet_center + svt::Vector(0.0, 0.0, 0.5f),
                                  tet_center,
                                  svt::Vector(0, 1, 0),
                                  45.0f);
    auto tet_shading = svt::Shading(svt::Colors::White);
    auto tet_ui_parameters = svt::UIParameters();
    auto canvas_tet = scene.create_canvas_3d("", SIZE, SIZE);
    canvas_tet->camera(tet_camera)
               .shading(tet_shading)
               .ui_parameters(tet_ui_parameters);

    const std::size_t n_frames = 5;
    for(std::size_t i=0; i<n_frames; ++i)
    {
        float angle = 2 * PI * i / n_frames;

        auto frame_rand = canvas_rand->create_frame();
        frame_rand->add_mesh(mesh_rand, svt::Transforms::translate({std::cos(angle), std::sin(angle), 0}));

        auto mesh_primitives = scene.create_mesh();
        mesh_primitives->layer_id("Primitives");
        mesh_primitives->add_disc(svt::Color(0, 1, 0),
                                 svt::Transforms::scale(0.2f + 0.2f * (1 + std::cos(angle))),
                                 10, false, true);
        mesh_primitives->add_cube(svt::Color(0, 0, 1), svt::Transforms::translate({-1.0f, -1.0f, -3.0f}));
        frame_rand->add_mesh(mesh_primitives);

        auto mesh_noise = scene.create_mesh();
        mesh_noise->shared_color(svt::Color(1.0, 0.0, 0.0))
                   .layer_id("Noise");
        mesh_noise->add_cylinder();
        mesh_noise->apply_transform(svt::Transforms::scale({0.02f, 0.1f, 0.1f}));
        mesh_noise->apply_transform(svt::Transforms::rotation_to_align_x_to_axis({0.5f, 0.5f, 0.5f}));
        svt::VectorBuffer positions(16, 3);
        for(std::size_t j=0; j<16*3; ++j){
            auto value = (j%4) / 4.0f;
            positions(j/3, j%3) = 2*value - 1;
        }
        mesh_noise->enable_instancing(positions);
        frame_rand->add_mesh(mesh_noise);

        auto frame_tet = canvas_tet->create_frame("", tet_center);
        frame_tet->add_mesh(model_mesh, svt::Transforms::rotation_about_z(std::cos(angle)));
    }

    canvas_rand->set_layer_settings({
        {"Test", svt::LayerSettings().filled(true)},
        {"Primitives", svt::LayerSettings().filled(false)},
        {"Noise", svt::LayerSettings().filled(false).opacity(0.5f)}
    });

    test::assert_equal(scene.to_json(), "scene", result);

    scene.clear_script();
    auto frame_tet = canvas_tet->create_frame("", tet_center);
    frame_tet->add_mesh(model_mesh, svt::Transforms::rotation_about_z(std::cos(2*PI)));

    test::assert_equal(scene.to_json(), "scene_cleared", result);

    return result;
}