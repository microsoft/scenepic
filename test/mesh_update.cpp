// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "scenepic.h"
#include "scenepic_tests.h"

namespace sp = scenepic;

int test_mesh_update()
{
  int result = EXIT_SUCCESS;
  sp::Scene scene;

  auto mesh = scene.create_mesh("base");
  mesh->add_triangle(test::COLOR);

  sp::VectorBuffer positions(3, 3);
  positions << 0, 0, 0, 1, 0, 0, 0, 0, 1;

  sp::VectorBuffer normals(3, 3);
  normals << 0, -1, 0, 0, -1, 0, 0, -1, 0;

  sp::VectorBuffer colors(3, 3);
  colors << 1, 0, 0, 0, 1, 0, 0, 0, 1;

  auto update = scene.update_mesh_positions("base", positions, "update0");

  test::assert_equal(update->to_json(), "update0", result);

  update = scene.update_mesh("base", positions, normals, colors, "update1");

  test::assert_equal(update->to_json(), "update1", result);

  sp::VertexBuffer keyframe_buffer = update->vertex_buffer();
  keyframe_buffer.topLeftCorner(1, 3) << 0, 1, 1;

  update->quantize(1, 6.0f, keyframe_buffer);

  test::assert_equal(update->to_json(), "update_quantized", result);

  sp::VectorBuffer instance_pos(3, 3);
  instance_pos << 0, 1, 2, 2, 0, 1, 1, 0, 2;

  sp::QuaternionBuffer instance_rot(3, 4);
  instance_rot << 0.11, 0.22, 0.46, 0.85, 0.46, -0.12, -0.22, 0.85, 0.22, -0.12,
    0.46, 0.85;

  sp::VectorBuffer instance_c(3, 3);
  instance_c << 1, 0, 0, 0, 1, 0, 0, 0, 1;

  mesh->enable_instancing(instance_pos, instance_rot, instance_c);

  instance_pos.row(0) << 1, 1, 0;

  update = scene.update_mesh_positions("base", instance_pos, "update2");

  test::assert_equal(update->to_json(), "update2", result);

  instance_pos.row(1) << 1, 0, 1;
  instance_rot.row(0) << 0.24, 0.24, 0.06, 0.94;
  instance_c.row(0) << 0.5, 0, 0;

  update = scene.update_instanced_mesh(
    "base", instance_pos, instance_rot, instance_c, "update3");

  test::assert_equal(update->to_json(), "update3", result);

  return result;
}