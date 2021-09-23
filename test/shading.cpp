#include "shading.h"
#include "svt_tests.h"

int test_shading()
{
    int result = EXIT_SUCCESS;
    svt::Shading shading;
    test::assert_equal(shading.to_json(), "shading", result);
    return result;
}