// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "scenepic.h"
#include "scenepic_tests.h"

namespace sp = scenepic;

int test_primitives()
{
  int result = EXIT_SUCCESS;
  sp::Scene scene;
  auto texture = scene.create_image("uv");
  texture->load(test::asset_path("uv.png"));

  auto mesh = scene.create_mesh("triangle");
  mesh->add_triangle(test::COLOR);
  test::assert_equal(mesh->to_json(), "triangle", result);

  mesh = scene.create_mesh("quad");
  mesh->add_quad(test::COLOR);
  test::assert_equal(mesh->to_json(), "quad", result);

  mesh = scene.create_mesh("cube");
  mesh->add_cube(test::COLOR);
  test::assert_equal(mesh->to_json(), "cube", result);

  mesh = scene.create_mesh("cube_texture");
  mesh->texture_id(texture->image_id());
  mesh->add_cube();
  test::assert_equal(mesh->to_json(), "cube_texture", result);

  mesh = scene.create_mesh("thickline");
  mesh->add_thickline(test::COLOR);
  test::assert_equal(mesh->to_json(), "thickline", result);

  mesh = scene.create_mesh("cone");
  mesh->add_cone(test::COLOR);
  test::assert_equal(mesh->to_json(), "cone", result);

  mesh = scene.create_mesh("trunc_cone");
  mesh->add_cone(test::COLOR, sp::Transform::Identity(), 0.7f);
  test::assert_equal(mesh->to_json(), "trunc_cone", result);

  mesh = scene.create_mesh("coordinate_axes_0");
  mesh->add_coordinate_axes();
  test::assert_equal(mesh->to_json(), "coordinate_axes_0", result);

  mesh = scene.create_mesh("coordinate_axes_1");
  mesh->shared_color(sp::Colors::White);
  mesh->add_cube();
  mesh->add_coordinate_axes();
  test::assert_equal(mesh->to_json(), "coordinate_axes_1", result);

  mesh = scene.create_mesh("camera_frustum");
  mesh->add_camera_frustum(test::COLOR);
  test::assert_equal(mesh->to_json(), "camera_frustum", result);

  mesh = scene.create_mesh("disc");
  mesh->add_disc(test::COLOR);
  test::assert_equal(mesh->to_json(), "disc", result);

  mesh = scene.create_mesh("sphere");
  mesh->add_sphere(test::COLOR);
  test::assert_equal(mesh->to_json(), "sphere", result);

  mesh = scene.create_mesh("icosphere");
  mesh->add_icosphere(test::COLOR);
  test::assert_equal(mesh->to_json(), "icosphere", result);

  mesh = scene.create_mesh("icosphere_texture");
  mesh->texture_id(texture->image_id());
  mesh->add_icosphere(sp::Color::None(), sp::Transform::Identity(), 1);
  test::assert_equal(mesh->to_json(), "icosphere_texture", result);

  mesh = scene.create_mesh("uv_sphere");
  mesh->add_uv_sphere(test::COLOR);
  test::assert_equal(mesh->to_json(), "uv_sphere", result);

  mesh = scene.create_mesh("point_cloud");
  sp::VectorBuffer positions(125, 3);
  for (int i = 0, x = 0; x < 5; ++x)
  {
    for (int y = 0; y < 5; ++y)
    {
      for (int z = 0; z < 5; ++z, ++i)
      {
        positions(i, 0) = (float)x;
        positions(i, 1) = (float)y;
        positions(i, 2) = (float)z;
      }
    }
  }
  positions = (positions.array() / 2) - 1;
  mesh->add_cube(test::COLOR);
  mesh->apply_transform(sp::Transforms::scale(0.01f));
  mesh->enable_instancing(positions);
  test::assert_equal(mesh->to_json(), "point_cloud", result);

  mesh = scene.create_mesh("line_cloud");
  sp::VectorBuffer end_points;
  end_points = positions * 10;
  mesh->add_lines(positions, end_points, test::COLOR);
  test::assert_equal(mesh->to_json(), "line_cloud", result);

  return result;
}
