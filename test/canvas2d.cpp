// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "canvas2d.h"

#include "scene.h"
#include "scenepic_tests.h"

int test_canvas2d()
{
  int result = EXIT_SUCCESS;

  scenepic::Scene scene;
  auto canvas2d = scene.create_canvas_2d("canvas2d");
  auto frame2d = canvas2d->create_frame();
  frame2d->add_circle(0, 0, 5);

  frame2d = canvas2d->create_frame();
  frame2d->add_rectangle(5, 6, 7, 8);

  frame2d = canvas2d->create_frame();
  frame2d->add_text("test", 1, 1);

  frame2d = canvas2d->create_frame();
  frame2d->add_image("rand");

  test::assert_equal(canvas2d->to_json(), "canvas2d", result);

  canvas2d->clear_script();
  frame2d = canvas2d->create_frame();
  scenepic::CoordinateBuffer positions(3, 2);
  positions << 0, 0, 1, 1, 2, 2;
  frame2d->add_line(positions);

  test::assert_equal(canvas2d->to_json(), "canvas2d_cleared", result);

  return result;
}