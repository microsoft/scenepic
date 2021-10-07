// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "canvas3d.h"

#include "scene.h"
#include "scenepic_tests.h"
#include "transforms.h"

int test_canvas3d()
{
  int result = EXIT_SUCCESS;

  scenepic::Scene scene;
  auto cube_mesh = scene.create_mesh("cube");
  cube_mesh->add_cube(test::COLOR);

  auto cone_mesh = scene.create_mesh("cone");
  cone_mesh->add_cone(test::COLOR);

  auto disc_mesh = scene.create_mesh("disc");
  disc_mesh->add_disc(test::COLOR);

  auto icosphere_mesh = scene.create_mesh("icosphere");
  icosphere_mesh->add_icosphere(test::COLOR);

  auto cylinder_mesh = scene.create_mesh("cylinder");
  cylinder_mesh->add_cylinder(test::COLOR);

  auto canvas3d = scene.create_canvas_3d("canvas3d");
  auto frame3d = canvas3d->create_frame("", scenepic::Vector(1, 0, 0));
  frame3d->add_mesh(cube_mesh);

  frame3d = canvas3d->create_frame();
  frame3d->add_mesh(disc_mesh, scenepic::Transforms::scale(5));

  frame3d = canvas3d->create_frame();
  frame3d->add_mesh(icosphere_mesh);

  frame3d = canvas3d->create_frame();
  frame3d->add_mesh(cylinder_mesh);

  test::assert_equal(canvas3d->to_json(), "canvas3d", result);

  canvas3d->clear_script();
  frame3d = canvas3d->create_frame();
  frame3d->add_mesh(cone_mesh);

  test::assert_equal(canvas3d->to_json(), "canvas3d_cleared", result);

  return result;
}