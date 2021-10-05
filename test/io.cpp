// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "scenepic.h"
#include "scenepic_tests.h"

using scenepic::Scene;

int test_io()
{
  int result = EXIT_SUCCESS;
  scenepic::Scene scene;

  auto image = scene.create_image("texture");
  image->load(test::asset_path("PolarBear.png"));

  auto mesh = scene.create_mesh("cube");
  mesh->texture_id(image->image_id());
  auto mesh_info = scenepic::load_obj(test::asset_path("cube.obj"));
  mesh->add_mesh(mesh_info);

  test::assert_equal(mesh->to_json(), "io", result);

  return result;
}