// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "ui_parameters.h"

#include "scenepic_tests.h"

int test_ui_parameters()
{
  int result = EXIT_SUCCESS;
  scenepic::UIParameters ui_parameters;
  test::assert_equal(ui_parameters.to_json(), "ui_parameters", result);
  return result;
}