// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "scenepic_tests.h"

#include "base64.h"
#include "compression.h"
#include "internal.h"

#include <functional>

namespace sp = scenepic;

namespace test
{
  const std::string ASSET_DIR = "assets";

  inline char sep()
  {
#if defined(_WIN32) || defined(WIN32)
    return '\\';
#else
    return '/';
#endif
  }

  std::string asset_path(const std::string& asset_name)
  {
    return ASSET_DIR + sep() + asset_name;
  }

  void assert_near(
    float actual,
    float expected,
    int& result,
    const std::string& tag,
    float epsilon)
  {
    if (std::abs(actual - expected) > epsilon)
    {
      result = EXIT_FAILURE;
      std::cerr << tag << " is incorrect: " << actual << " != " << expected
                << std::endl;
    }
  }

  Eigen::MatrixXf to_matrix(const std::string& buffer)
  {
    std::vector<std::uint8_t> bytes(buffer.begin(), buffer.end());
    Eigen::MatrixXf values = sp::decompress_matrix<Eigen::MatrixXf>(bytes);
    return values;
  }

  void assert_buffer_equal(
    const sp::Json::Value& actual,
    const sp::Json::Value& expected,
    int& result,
    const std::string& tag,
    float epsilon)
  {
    std::string actual_buffer = scenepic::base64_decode(actual.asString());
    std::string expected_buffer = scenepic::base64_decode(expected.asString());
    Eigen::MatrixXf actual_matrix = to_matrix(actual_buffer);
    Eigen::MatrixXf expected_matrix = to_matrix(expected_buffer);

    assert_equal(
      actual_matrix.rows(), expected_matrix.rows(), result, tag + " rows");
    assert_equal(
      actual_matrix.cols(), expected_matrix.cols(), result, tag + " cols");

    for (auto row = 0; row < actual_matrix.rows(); ++row)
    {
      for (auto col = 0; col < actual_matrix.cols(); ++col)
      {
        assert_near(
          actual_matrix(row, col),
          expected_matrix(row, col),
          result,
          tag + "[" + std::to_string(row) + ", " + std::to_string(col) + "]",
          epsilon);

        if (result == EXIT_FAILURE)
        {
          std::cerr << tag << " is incorrect: " << std::endl
                    << actual.asString() << std::endl
                    << "!=" << std::endl
                    << expected.asString() << std::endl;
          break;
        }
      }

      if (result == EXIT_FAILURE)
      {
        break;
      }
    }
  }

  std::vector<float> decompress_buffer(
    const std::string& raw_buffer, float min_value, float max_value)
  {
    std::vector<std::uint8_t> buffer(raw_buffer.begin(), raw_buffer.end());
    sp::FixedPointVertexBuffer fp_buffer =
      sp::decompress_matrix<sp::FixedPointVertexBuffer>(buffer);
    sp::VertexBuffer vertex_buffer = fp_buffer.cast<float>();
    float scale = (max_value - min_value) / 65535;
    vertex_buffer = (vertex_buffer.array() * scale + min_value);
    return std::vector<float>(
      vertex_buffer.data(), vertex_buffer.data() + vertex_buffer.size());
  }

  void assert_quantized_buffer_equal(
    const sp::Json::Value& actual,
    const sp::Json::Value& expected,
    int& result,
    const std::string& tag,
    float epsilon)
  {
    std::string actual_buffer =
      scenepic::base64_decode(actual["QuantizedBuffer"].asString());
    std::string expected_buffer =
      scenepic::base64_decode(expected["QuantizedBuffer"].asString());

    std::vector<float> actual_values = decompress_buffer(
      actual_buffer,
      actual["MinValue"].asFloat(),
      actual["MaxValue"].asFloat());
    std::vector<float> expected_values = decompress_buffer(
      expected_buffer,
      expected["MinValue"].asFloat(),
      expected["MaxValue"].asFloat());

    assert_equal(
      actual_values.size(), expected_values.size(), result, tag + " size");
    if (result == EXIT_FAILURE)
    {
      return;
    }

    for (std::size_t i = 0; i < actual_values.size(); ++i)
    {
      assert_near(
        actual_values[i],
        expected_values[i],
        result,
        tag + "[" + std::to_string(i) + "]",
        epsilon);
      if (result == EXIT_FAILURE)
      {
        std::cerr << tag << " is incorrect: " << std::endl
                  << actual << std::endl
                  << "!=" << std::endl
                  << expected << std::endl;
        break;
      }
    }
  }

  void assert_json_equal(
    const sp::Json::Value& actual,
    const sp::Json::Value& expected,
    int& result,
    const std::string& tag,
    float epsilon)
  {
    assert_equal(actual.type(), expected.type(), result, tag + " type");
    if (expected.type() != actual.type())
    {
      result = EXIT_FAILURE;
      std::cerr << "type mismatch: " << actual.type()
                << " != " << expected.type() << std::endl;
      return;
    }

    if (actual.type() == sp::Json::ValueType::objectValue)
    {
      assert_equal(actual.size(), expected.size(), result, tag + " size");
      if (result == EXIT_FAILURE)
      {
        return;
      }

      auto actual_members = actual.getMemberNames();
      auto expected_members = expected.getMemberNames();
      for (std::size_t i = 0; i < actual_members.size(); ++i)
      {
        assert_equal(
          actual_members[i], expected_members[i], result, tag + " member");
        const std::string& member = actual_members[i];
        if (
          member == "VertexBuffer" || member == "Transform" ||
          member == "CoordinateBuffer" || member == "CircleBuffer" ||
          member == "InstanceBuffer" || member == "ValueBuffer" ||
          member == "LayerBuffer" || member == "Projection")
        {
          assert_buffer_equal(
            actual[member],
            expected[member],
            result,
            tag + "." + member,
            epsilon);
        }
        else if (member == "QuantizedBuffer")
        {
          assert_quantized_buffer_equal(
            actual, expected, result, tag + ".QuantizedBuffer", epsilon);
        }
        else
        {
          assert_json_equal(
            actual[member],
            expected[member],
            result,
            tag + "." + member,
            epsilon);
        }
      }
    }
    else if (actual.type() == sp::Json::ValueType::arrayValue)
    {
      assert_equal(actual.size(), expected.size(), result, tag + " size");
      if (result == EXIT_FAILURE)
      {
        return;
      }

      for (sp::Json::ArrayIndex i = 0; i < actual.size(); ++i)
      {
        assert_json_equal(
          actual[i],
          expected[i],
          result,
          tag + "[" + std::to_string(i) + "]",
          epsilon);
        if (result == EXIT_FAILURE)
        {
          return;
        }
      }
    }
    else
    {
      if (actual.type() == sp::Json::ValueType::realValue)
      {
        assert_near(actual.asFloat(), expected.asFloat(), result, tag, epsilon);
      }
      else
      {
        assert_equal(actual, expected, result, tag);
      }
    }
  }

  void assert_equal(
    const sp::JsonValue& value,
    const std::string& expected_name,
    int& result,
    float epsilon)
  {
    sp::Json::Value actual = scenepic_to_json(value);
    sp::Json::Value expected;
    std::ifstream expected_stream(asset_path(expected_name) + ".json");
    expected_stream >> expected;
    assert_json_equal(actual, expected, result, expected_name, epsilon);

    if (result == EXIT_FAILURE)
    {
      std::cerr << actual << std::endl;
    }
  }
} // namespace test

typedef std::function<int()> TestFunction;

int main(int argc, char** argv)
{
  std::map<std::string, TestFunction> tests;
  tests["audio"] = test_audio;
  tests["base64"] = test_base64;
  tests["camera"] = test_camera;
  tests["canvas2d"] = test_canvas2d;
  tests["canvas3d"] = test_canvas3d;
  tests["drop_down_menu"] = test_drop_down_menu;
  tests["frame2d"] = test_frame2d;
  tests["frame3d"] = test_frame3d;
  tests["graph"] = test_graph;
  tests["image"] = test_image;
  tests["io"] = test_io;
  tests["label"] = test_label;
  tests["layer_settings"] = test_layer_settings;
  tests["matrix"] = test_matrix;
  tests["mesh_update"] = test_mesh_update;
  tests["primitives"] = test_primitives;
  tests["quantization"] = test_quantization;
  tests["scene"] = test_scene;
  tests["shading"] = test_shading;
  tests["stencil"] = test_stencil;
  tests["text_panel"] = test_text_panel;
  tests["transforms"] = test_transforms;
  tests["ui_parameters"] = test_ui_parameters;
  tests["video"] = test_video;

  if (argc == 2)
  {
    std::string test(argv[1]);
    if (tests.count(test))
    {
      return tests[test]();
    }
    else
    {
      std::cout << "Invalid test: " << test << std::endl;
      return EXIT_FAILURE;
    }
  }
  else
  {
    int result = EXIT_SUCCESS;
    int num_passes = 0;
    int num_tests = 0;
    for (auto& test : tests)
    {
      ++num_tests;
      std::cout << "Running " << test.first << "..." << std::endl;
      if (test.second())
      {
        result = EXIT_FAILURE;
        std::cout << test.first << " failed." << std::endl;
      }
      else
      {
        num_passes++;
        std::cout << test.first << " passed." << std::endl;
      }
    }

    std::cout << num_passes << "/" << num_tests << " passed." << std::endl;

    return result;
  }
}