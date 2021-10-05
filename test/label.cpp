// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "scene.h"
#include "scenepic_tests.h"

int test_label()
{
  int result = EXIT_SUCCESS;
  scenepic::Scene scene;
  auto label = scene.create_label();
  test::assert_equal(scene.to_json(), "label", result);
  return result;
}