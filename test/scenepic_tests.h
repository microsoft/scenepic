// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#ifndef _SCPIC_TESTS_H_
#define _SCPIC_TESTS_H_

#include "color.h"
#include "json_value.h"

#include <Eigen/Core>
#include <fstream>
#include <iostream>
#include <string>

int test_audio();
int test_base64();
int test_camera();
int test_canvas2d();
int test_canvas3d();
int test_drop_down_menu();
int test_frame2d();
int test_frame3d();
int test_graph();
int test_image();
int test_io();
int test_label();
int test_layer_settings();
int test_matrix();
int test_mesh_update();
int test_primitives();
int test_quantization();
int test_scene();
int test_shading();
int test_stencil();
int test_text_panel();
int test_transforms();
int test_ui_parameters();
int test_video();

namespace test
{
  const float EPSILON = 1e-5f;
  const scenepic::Color COLOR(0.83863144f, 0.39671423f, 0.77389568f);
  std::string asset_path(const std::string& asset_name);
  void assert_equal(
    const scenepic::JsonValue& actual,
    const std::string& expected_name,
    int& result,
    float epsilon = EPSILON);
  void assert_close(
    float actual,
    float expected,
    int& result,
    const std::string& tag,
    float epsilon = EPSILON);
  void assert_near(
    float actual,
    float expected,
    int& result,
    const std::string& tag,
    float epsilon = EPSILON);

  template<typename T>
  void assert_equal(T actual, T expected, int& result, const std::string& tag)
  {
    if (actual != expected)
    {
      result = EXIT_FAILURE;
      std::cerr << tag << " is incorrect: " << actual << " != " << expected
                << std::endl;
    }
  }

  template<typename T>
  void assert_equal(
    const std::vector<T>& actual,
    const std::vector<T>& expected,
    int& result,
    const std::string& tag)
  {
    assert_equal(actual.size(), expected.size(), result, tag + " size");
    if (result == EXIT_SUCCESS)
    {
      for (std::size_t i = 0; i < expected.size(); ++i)
      {
        assert_equal(
          actual[i], expected[i], result, tag + "[" + std::to_string(i) + "]");
        if (result == EXIT_FAILURE)
        {
          break;
        }
      }
    }
  }

  template<typename T>
  void assert_lessthan(
    const T& actual, const T& expected, int& result, const std::string& tag)
  {
    if (expected < actual)
    {
      result = EXIT_FAILURE;
      std::cerr << tag << " is incorrect: " << actual << " >= " << expected
                << std::endl;
    }
  }

  template<typename Derived>
  void assert_allclose(
    const Eigen::MatrixBase<Derived>& actual,
    const Eigen::MatrixBase<Derived>& expected,
    int& result,
    const std::string& tag,
    typename Derived::Scalar tolerance = 1e-6)
  {
    typename Derived::Scalar max_diff =
      (actual - expected).array().abs().maxCoeff();
    if (max_diff > tolerance)
    {
      result = EXIT_FAILURE;
      std::cerr << tag << " is incorrect, maximum difference is " << max_diff
                << " which is not within tolerance of " << tolerance
                << std::endl;
      std::cerr << "actual: " << actual << std::endl;
      std::cerr << "expected: " << expected << std::endl;
    }
  }

  template<typename Derived>
  void assert_allclose(
    const Eigen::ArrayBase<Derived>& actual,
    const Eigen::ArrayBase<Derived>& expected,
    int& result,
    const std::string& tag,
    typename Derived::Scalar tolerance = 1e-6)
  {
    typename Derived::Scalar max_diff = (actual - expected).abs().maxCoeff();
    if (max_diff > tolerance)
    {
      result = EXIT_FAILURE;
      std::cerr << tag << " is incorrect, maximum difference is " << max_diff
                << " which is not within tolerance of " << tolerance
                << std::endl;
      std::cerr << "actual: " << actual << std::endl;
      std::cerr << "expected: " << expected << std::endl;
    }
  }

  template<typename Derived>
  void assert_range(
    const Eigen::MatrixBase<Derived>& actual,
    const float min,
    const float max,
    int& result,
    const std::string& tag)
  {
    if (actual.maxCoeff() > max || actual.minCoeff() < min)
    {
      result = EXIT_FAILURE;
      std::cerr << tag << "Random matrix is not within range " << min << " and "
                << max << std::endl;
      std::cerr << "actual: " << actual << std::endl;
    }
  }

  template<typename Derived0, typename Derived1, typename Derived2>
  void assert_range(
    const Eigen::MatrixBase<Derived0>& actual,
    const Eigen::MatrixBase<Derived1>& min,
    const Eigen::MatrixBase<Derived2>& max,
    int& result,
    const std::string& tag)
  {
    assert_equal(min.size(), max.size(), result, tag + " size");
    if (result == EXIT_SUCCESS)
    {
      for (std::size_t i = 0; i < min.size(); ++i)
      {
        assert_range(
          actual.col(i),
          min[i],
          max[i],
          result,
          tag + "[" + std::to_string(i) + "]");
        if (result == EXIT_FAILURE)
        {
          break;
        }
      }
    }
  }

} // namespace test

#endif