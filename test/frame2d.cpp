// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "scene.h"
#include "scenepic_tests.h"

int test_frame2d()
{
  int result = EXIT_SUCCESS;

  scenepic::Scene scene;
  auto canvas2d = scene.create_canvas_2d();
  auto frame2d = canvas2d->create_frame();
  frame2d->add_circle(0, 0, 5.0);

  test::assert_equal(frame2d->to_json(), "frame2d", result);

  return result;
}