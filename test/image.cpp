// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "scene.h"
#include "scenepic_tests.h"

int test_image()
{
  int result = EXIT_SUCCESS;

  scenepic::Scene scene;
  auto image = scene.create_image("rand");
  image->load(test::asset_path("rand.png"));
  test::assert_equal(image->to_json(), "image", result);

  auto mesh = scene.create_mesh("image");
  mesh->texture_id(image->image_id());
  mesh->add_image();
  test::assert_equal(mesh->to_json(), "image_mesh", result);

  return result;
}