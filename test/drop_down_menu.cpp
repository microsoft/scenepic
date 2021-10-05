// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "scene.h"
#include "scenepic_tests.h"

int test_drop_down_menu()
{
  int result = EXIT_SUCCESS;

  scenepic::Scene scene;
  auto drop_down_menu = scene.create_drop_down_menu("", "DropDown");
  drop_down_menu->items({"one", "two", "three"});
  test::assert_equal(drop_down_menu->to_json(), "drop_down_menu", result);

  return result;
}