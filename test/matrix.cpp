// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "matrix.h"

#include "scenepic_tests.h"

namespace sp = scenepic;

int test_matrix()
{
  int result = EXIT_SUCCESS;

  Eigen::MatrixXf actual(3, 4);

  actual << 0, -1, 3, -0.03, 0, 2, 0, 3, 0.51, -0.22, 1, 2;

  sp::Color4 vec(-0.87, 1, 0, -1);

  Eigen::MatrixXf expected(4, 4);
  expected << 0, -1, 3, -0.03, 0, 2, 0, 3, 0.51, -0.22, 1, 2, -0.87, 1, 0, -1;

  sp::append_row(actual, vec);
  test::assert_allclose(actual, expected, result, "append_row");

  Eigen::MatrixXf top(2, 4);
  Eigen::MatrixXf bottom(2, 4);
  top << 0, -1, 3, -0.03, 0, 2, 0, 3;
  bottom << 0.51, -0.22, 1, 2, -0.87, 1, 0, -1;

  sp::append_matrix(top, bottom);
  test::assert_allclose(top, expected, result, "append_matrix");

  Eigen::Array<std::uint32_t, Eigen::Dynamic, 1> expectedArray(8);
  expectedArray << 0, 2, 4, 6, 8, 10, 12, 14;
  test::assert_allclose(sp::arange(0, 17, 2), expectedArray, result, "arange");

  expected << 0, -1, 3, -0.03, 0, 1, 3, 2.97, 0.51, 0.78, 4, 4.97, -0.36, 1.78,
    4, 3.97;
  test::assert_allclose(
    sp::rowwise_cumsum(actual), expected, result, "rowwise_cumsum");

  Eigen::Array<std::uint32_t, Eigen::Dynamic, 1> actualArray(8);

  actualArray << 10, 12, 14, 0, 2, 4, 6, 8;

  test::assert_allclose(
    sp::roll(actualArray, 5), expectedArray, result, "roll");

  Eigen::Index i = 5;
  Eigen::MatrixXf actualMatrix(i, 3);

  Eigen::MatrixXf expectedMatrix(i, 3);
  expectedMatrix << 1, 5, 0, 2.5, 6, 1.25, 4, 7, 2.5, 5.5, 8, 3.75, 7, 9, 5;

  sp::linspace(actualMatrix, sp::Vector(1, 5, 0), sp::Vector(7, 9, 5));
  test::assert_allclose(actualMatrix, expectedMatrix, result, "linspace");

  actualMatrix =
    sp::linspace<sp::VectorBuffer>(i, sp::Vector(1, 5, 0), sp::Vector(7, 9, 5));
  test::assert_allclose(actualMatrix, expectedMatrix, result, "linspace");

  sp::VectorBuffer actualRandom = sp::random<sp::VectorBuffer>(3, 3, -1, 7);
  test::assert_range(actualRandom, -1, 7, result, "range");

  actualRandom = sp::random<sp::VectorBuffer>(
    3, 3, sp::Vector(0, 1, 2), sp::Vector(3, 4, 5));
  test::assert_range(
    actualRandom, sp::Vector(0, 1, 2), sp::Vector(3, 4, 5), result, "range");

  actualRandom = sp::random<sp::Vector>(0.5, 4.6);
  test::assert_range(actualRandom, 0.5, 4.6, result, "range");

  std::string actualJson = sp::matrix_to_json(expected);
  std::string expectedJson =
    "eAEBQAC//wAAAAAAAAAAXI8CP+xRuL4AAIC/AACAPxSuRz8K1+M/"
    "AABAQAAAQEAAAIBAAACAQI/C9bx7FD5APQqfQHsUfkARKhIrBAAAAAQ=";
  test::assert_equal(actualJson, expectedJson, result, "json");

  return result;
}
