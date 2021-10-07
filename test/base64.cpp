// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "base64.h"

#include "scenepic_tests.h"

#include <fstream>

int test_base64()
{
  int result = EXIT_SUCCESS;

  std::ifstream file(test::asset_path("rand.png"), std::ios::binary);

  file.seekg(0, std::ios::end);
  auto file_size = file.tellg();
  file.seekg(0, std::ios::beg);

  std::vector<std::uint8_t> expected(static_cast<std::size_t>(file_size));
  file.read((char*)&expected[0], file_size);

  std::string encoded = scenepic::base64_encode(
    expected.data(), static_cast<unsigned int>(expected.size()));
  std::string decoded = scenepic::base64_decode(encoded);

  std::vector<std::uint8_t> actual(decoded.begin(), decoded.end());

  test::assert_equal(actual, expected, result, "base64");

  return result;
}