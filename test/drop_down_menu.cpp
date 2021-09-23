#include "scene.h"
#include "svt_tests.h"

int test_drop_down_menu()
{
    int result = EXIT_SUCCESS;

    svt::Scene scene;
    auto drop_down_menu = scene.create_drop_down_menu("", "DropDown");
    drop_down_menu->items({"one", "two", "three"});
    test::assert_equal(drop_down_menu->to_json(), "drop_down_menu", result);

    return result;
}