// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "scene.h"
#include "scenepic_tests.h"

int test_frame3d()
{
  int result = EXIT_SUCCESS;

  scenepic::Scene scene;
  auto cube_mesh = scene.create_mesh("cube");
  cube_mesh->add_cube(test::COLOR);

  auto canvas3d = scene.create_canvas_3d();
  auto frame3d = canvas3d->create_frame("", scenepic::Vector(1, 0, 0));
  frame3d->add_mesh(cube_mesh);

  test::assert_equal(frame3d->to_json(), "frame3d", result);

  return result;
}