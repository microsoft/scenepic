// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "scenepic.h"
#include "scenepic_tests.h"

namespace sp = scenepic;

int test_stencil()
{
  int result = EXIT_SUCCESS;
  auto hand_lo = sp::load_obj(test::asset_path("hand.obj"));
  auto hand_hi1 = hand_lo->subdivide(1, true);
  auto hand_hi2 = hand_lo->subdivide(2);

  sp::Scene scene;
  auto hand_mesh1 = scene.create_mesh("hand1");
  hand_mesh1->shared_color(sp::Colors::Pink);
  hand_mesh1->add_mesh(hand_hi1);
  auto hand_mesh2 = scene.create_mesh("hand2");
  hand_mesh2->shared_color(sp::Colors::Blue);
  hand_mesh2->add_mesh(hand_hi2);

  test::assert_equal(scene.to_json(), "loop_subdivision_stencil", result);
  return result;
}