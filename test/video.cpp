// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "scene.h"
#include "scenepic_tests.h"

int test_video()
{
  int result = EXIT_SUCCESS;

  scenepic::Scene scene;
  auto video = scene.create_video("test");
  video->load(test::asset_path("test.mp4"));
  test::assert_equal(video->to_json(), "video", result);

  return result;
}