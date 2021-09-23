#include "scene.h"
#include "svt_tests.h"

int test_label()
{
    int result = EXIT_SUCCESS;
    svt::Scene scene;
    auto label = scene.create_label();
    test::assert_equal(scene.to_json(), "label", result);
    return result;
}