// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "scenepic.h"
#include "scenepic_tests.h"

namespace sp = scenepic;

int test_layer_settings()
{
  int result = EXIT_SUCCESS;

  sp::Scene scene;
  auto canvas3d = scene.create_canvas_3d();
  auto canvas2d = scene.create_canvas_2d();

  auto mesh0 = scene.create_mesh("null");
  auto mesh1 = scene.create_mesh("one");
  mesh1->layer_id("one");
  auto mesh2 = scene.create_mesh("two");
  mesh2->layer_id("two");
  auto image0 = scene.create_image("null");
  auto image1 = scene.create_image("one");
  auto image2 = scene.create_image("two");
  auto video = scene.create_video();

  canvas2d->media_id(video->video_id());

  for (auto i = 0; i < 2; ++i)
  {
    auto frame3d = canvas3d->create_frame();
    frame3d->add_mesh(mesh0);
    frame3d->add_mesh(mesh1);
    frame3d->add_mesh(mesh2);

    auto frame2d = canvas2d->create_frame();
    frame2d->add_circle(0, 0, 1);
    frame2d->add_circle(
      0, 0, 2, sp::Colors::Black, 1.0f, sp::Color::None(), "one");
    frame2d->add_circle(
      0, 0, 3, sp::Colors::Black, 1.0f, sp::Color::None(), "two");

    frame2d->add_text("null", 0, 0);
    frame2d->add_text(
      "one", 100, 100, sp::Colors::White, 12.0f, "sans-serif", "one");
    frame2d->add_text(
      "two", 200, 200, sp::Colors::White, 12.0f, "sans-serif", "two");

    sp::CoordinateBuffer coords(2, 2);
    coords << 0, 0, 0, 0;
    frame2d->add_line(coords);
    frame2d->add_line(
      coords.array() + 1,
      sp::Colors::Black,
      1.0f,
      sp::Color::None(),
      false,
      "one");
    frame2d->add_line(
      coords.array() + 2,
      sp::Colors::Black,
      1.0f,
      sp::Color::None(),
      false,
      "two");

    frame2d->add_rectangle(0, 0, 0, 0);
    frame2d->add_rectangle(
      0, 0, 10, 10, sp::Colors::Black, 1.0f, sp::Color::None(), "one");
    frame2d->add_rectangle(
      0, 0, 20, 20, sp::Colors::Black, 1.0f, sp::Color::None(), "two");

    frame2d->add_image(image0->image_id());
    frame2d->add_image(
      image1->image_id(), "fit", 0.0f, 0.0f, 1.0f, false, "one");
    frame2d->add_image(
      image2->image_id(), "fit", 0.0f, 0.0f, 1.0f, false, "two");

    frame2d->add_video("manual", 0, 0);
    frame2d->add_video("manual", 1, 1, 1.0f, false, "one");
    frame2d->add_video("manual", 2, 2, 1.0f, false, "two");
  }

  std::map<std::string, sp::LayerSettings> layer_settings = {
    {"one", sp::LayerSettings().opacity(0.5).render_order(1)},
    {"two", sp::LayerSettings().opacity(0.9)}};

  canvas2d->set_layer_settings(layer_settings);
  canvas3d->set_layer_settings(layer_settings);

  test::assert_equal(canvas2d->to_json(), "layer_settings_2d", result);
  test::assert_equal(canvas3d->to_json(), "layer_settings_3d", result);

  return result;
}