// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "scene.h"
#include "scenepic_tests.h"

int test_audio()
{
  int result = EXIT_SUCCESS;

  scenepic::Scene scene;
  auto audio = scene.create_audio("hello");
  audio->load(test::asset_path("hello.mp3"));
  test::assert_equal(audio->to_json(), "audio", result);

  return result;
}