// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "graph.h"

#include "scene.h"
#include "scenepic_tests.h"

#include <algorithm>
#include <cmath>
#include <numeric>
#include <vector>

int test_graph()
{
  int result = EXIT_SUCCESS;

  std::vector<int> t(30);
  std::vector<float> cos_t;
  std::vector<float> sin_t;

  std::iota(t.begin(), t.end(), 0);
  std::transform(t.begin(), t.end(), std::back_inserter(cos_t), [](int t) {
    return static_cast<float>(std::cos(static_cast<double>(t) / 29.0));
  });
  std::transform(t.begin(), t.end(), std::back_inserter(sin_t), [](int t) {
    return static_cast<float>(std::sin(static_cast<double>(t) / 29.0));
  });

  scenepic::Scene scene;
  auto graph = scene.create_graph();
  graph->name_align("top").value_align("bottom").name_size(32).value_size(10);
  std::vector<scenepic::Graph::VerticalRule> rules;
  rules.emplace_back(10, scenepic::Colors::Red, 2.0f);
  rules.emplace_back(20, scenepic::Colors::Green, 1.0f);
  graph->add_sparkline("cos", cos_t, scenepic::Colors::Red, 1.0, rules);
  graph->add_sparkline("sin", sin_t, scenepic::Colors::Black, 2.0f);

  test::assert_equal(graph->to_json(), "graph", result);
  return result;
}