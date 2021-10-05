// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "scene.h"
#include "scenepic_tests.h"

namespace sp = scenepic;

namespace
{
  void integration(int& result)
  {
    // integration test
    sp::Scene scene;

    auto mesh = scene.create_mesh("base");
    mesh->add_triangle(test::COLOR);

    for (auto i = 0; i < 20; ++i)
    {
      sp::VectorBuffer positions(3, 3);
      positions << 0, 0, 0, 1, i * 0.05f, 0, 0, 1, 0;
      scene.update_mesh_without_normals("base", positions);
    }

    auto quantization_info = scene.quantize_updates(1e-5f);

    test::assert_equal(
      quantization_info["base"].keyframe_count, 2U, result, "keyframe_count");
    test::assert_equal(scene.to_json(), "compression", result);
  }

  void error_bound(int& result)
  {
    sp::Scene scene;
    auto mesh = scene.create_mesh("sphere");
    mesh->add_sphere(test::COLOR);

    sp::VectorBuffer positions = mesh->vertex_positions();
    positions = positions +
      sp::VectorBuffer::Random(positions.rows(), positions.cols()) * 0.01f;

    auto update = scene.update_mesh_without_normals("sphere", positions);

    float expected_error = 1e-4f;
    float range = expected_error * 65535;
    update->quantize(0, range, mesh->vertex_buffer().leftCols(6));

    sp::VertexBuffer actual =
      update->unquantize() + mesh->vertex_buffer().leftCols(6);
    auto diff = (actual - update->vertex_buffer());

    test::assert_lessthan(
      diff.maxCoeff() - diff.minCoeff(),
      expected_error,
      result,
      "Compression @ " + std::to_string(expected_error));

    // test error bounds
  }
} // namespace

int test_compression()
{
  int result = EXIT_SUCCESS;

  integration(result);
  error_bound(result);

  return result;
}