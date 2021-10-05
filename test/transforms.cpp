// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "transforms.h"

#include "scenepic_tests.h"

#include <cmath>

namespace sp = scenepic;

namespace
{
  const float PI = 3.1415927f;
}

int test_transforms()
{
  int result = EXIT_SUCCESS;

  sp::Transform expected;

  sp::Vector vec(0.1f, 0.2f, 0.3f);

  expected << 0.1f, 0, 0, 0, 0, 0.1f, 0, 0, 0, 0, 0.1f, 0, 0, 0, 0, 1;

  test::assert_allclose(
    sp::Transforms::scale(0.1f), expected, result, "scale_1");

  expected << 0.1f, 0, 0, 0, 0, 0.2f, 0, 0, 0, 0, 0.3f, 0, 0, 0, 0, 1;

  test::assert_allclose(
    sp::Transforms::scale(vec), expected, result, "scale_3");

  expected << 1, 0, 0, 0.1f, 0, 1, 0, 0.2f, 0, 0, 1, 0.3f, 0, 0, 0, 1;

  test::assert_allclose(
    sp::Transforms::translate(vec), expected, result, "translate");

  float angle = 60.0f * PI / 180.0f;
  float cos = std::cos(angle);
  float sin = std::sin(angle);
  expected << 1, 0, 0, 0, 0, cos, -sin, 0, 0, sin, cos, 0, 0, 0, 0, 1;

  test::assert_allclose(
    sp::Transforms::rotation_about_x(angle),
    expected,
    result,
    "rotation_about_x");

  expected << cos, 0, sin, 0, 0, 1, 0, 0, -sin, 0, cos, 0, 0, 0, 0, 1;

  test::assert_allclose(
    sp::Transforms::rotation_about_y(angle),
    expected,
    result,
    "rotation_about_y");

  expected << cos, -sin, 0, 0, sin, cos, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1;

  test::assert_allclose(
    sp::Transforms::rotation_about_z(angle),
    expected,
    result,
    "rotation_about_z");

  expected << 0.936293f, -0.289629f, 0.198669f, 0, 0.312992f, 0.944703f,
    -0.0978434f, 0, -0.159345f, 0.153792f, 0.97517f, 0, 0, 0, 0, 1;

  test::assert_allclose(
    sp::Transforms::euler_angles_to_matrix(vec),
    expected,
    result,
    "euler_angles_to_matrix");

  expected << 0.267261f, -0.534522f, -0.801784f, 0, 0.534522f, 0.774542f,
    -0.338187f, 0, 0.801784f, -0.338187f, 0.492719f, 0, 0, 0, 0, 1;

  test::assert_allclose(
    sp::Transforms::rotation_to_align_x_to_axis(vec),
    expected,
    result,
    "rotation_to_align_x_to_axis");

  sp::Quaternion expected_quat(0, -0.503627f, 0.335751f, 0.796009f);

  test::assert_allclose(
    sp::Transforms::quaternion_to_align_x_to_axis(vec),
    expected_quat,
    result,
    "quaternion_to_align_x_to_axis");

  test::assert_allclose(
    sp::Transforms::quaternion_to_matrix(expected_quat),
    expected,
    result,
    "quaternion_to_matrix",
    1.0e-5f);

  double fov_y_degrees = 45;
  double aspect_ratio = 16.0 / 9.0;
  double znear = 1;
  double zfar = 100;
  expected << 1.357995f, 0, 0, 0, 0, 2.414214f, 0, 0, 0, 0, -1.020202f,
    -2.020202f, 0, 0, -1, 0;

  test::assert_allclose(
    sp::Transforms::gl_projection(fov_y_degrees, aspect_ratio, znear, zfar),
    expected,
    result,
    "gl_projection");

  sp::Intrinsic intrinsic;
  intrinsic << 7.5438633e+03f, 1.1557838e+01f, 2.0555000e+03f, 0,
    7.5612310e+03f, 1.5035000e+03f, 0, 0, 1;
  int width = 4112;
  int height = 3008;
  expected << 3.6691942f, -5.6215166e-03f, 2.4319066e-04f, 0, 0, 5.0274143f,
    -3.3244680e-04f, 0, 0, 0, -1.0202020f, -2.0202019f, 0, 0, -1, 0;

  test::assert_allclose(
    sp::Transforms::gl_projection(intrinsic, width, height, znear, zfar),
    expected,
    result,
    "gl_projection");

  sp::Extrinsic extrinsic;
  extrinsic << 0.6245465f, 0.07013566f, 0.77783203f, -4.461637f, -0.12572001f,
    -0.973942f, 0.18876319f, -0.7838287f, 0.77080226f, -0.21568045f,
    -0.5994546f, -4.4914584f, 0, 0, 0, 1;
  expected << 0.62454647f, -0.12572001f, 0.77080226f, 6.149983f, -0.07013565f,
    0.9739419f, 0.21568044f, 1.4192035f, -0.77783203f, -0.1887632f, 0.5994546f,
    -0.92593664f, 0, 0, 0, 1;

  test::assert_allclose(
    sp::Transforms::gl_world_to_camera(extrinsic),
    expected,
    result,
    "gl_world_to_camera");

  return result;
}