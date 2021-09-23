#include "ui_parameters.h"
#include "svt_tests.h"

int test_ui_parameters()
{
    int result = EXIT_SUCCESS;
    svt::UIParameters ui_parameters;
    test::assert_equal(ui_parameters.to_json(), "ui_parameters", result);
    return result;
}