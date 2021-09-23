#include <cmath>
#include <algorithm>
#include <numeric>
#include <vector>

#include "scene.h"
#include "graph.h"
#include "svt_tests.h"

int test_graph()
{
    int result = EXIT_SUCCESS;

    std::vector<int> t(30);
    std::vector<float> cos_t;
    std::vector<float> sin_t;

    std::iota(t.begin(), t.end(), 0);
    std::transform(t.begin(), t.end(), std::back_inserter(cos_t),
                   [](int t){return static_cast<float>(std::cos(static_cast<double>(t) / 29.0));});
    std::transform(t.begin(), t.end(), std::back_inserter(sin_t),
                   [](int t){return static_cast<float>(std::sin(static_cast<double>(t) / 29.0));});

    svt::Scene scene;
    auto graph = scene.create_graph();
    graph->add_sparkline("cos", cos_t, svt::Colors::Red);
    graph->add_sparkline("sin", sin_t, svt::Colors::Black, 2.0f);

    test::assert_equal(graph->to_json(), "graph", result);
    return result;
}