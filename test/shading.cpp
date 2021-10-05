// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "shading.h"

#include "scenepic_tests.h"

int test_shading()
{
  int result = EXIT_SUCCESS;
  scenepic::Shading shading;
  test::assert_equal(shading.to_json(), "shading", result);
  return result;
}