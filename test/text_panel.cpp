#include "scene.h"
#include "svt_tests.h"

int test_text_panel()
{
    int result = EXIT_SUCCESS;

    svt::Scene scene;
    auto text_panel = scene.create_text_panel();
    text_panel->text("Text").title("Title");
    test::assert_equal(text_panel->to_json(), "text_panel", result);

    return result;
}