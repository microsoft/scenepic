#include "svt_tests.h"
#include "svt.h"

int test_stencil()
{
    int result = EXIT_SUCCESS;
    auto hand_lo = svt::load_obj(test::asset_path("hand.obj"));
    auto hand_hi1 = hand_lo->subdivide(1, true);
    auto hand_hi2 = hand_lo->subdivide(2);

    svt::Scene scene;
    auto hand_mesh1 = scene.create_mesh("hand1");
    hand_mesh1->shared_color(svt::Colors::Pink);
    hand_mesh1->add_mesh(hand_hi1);
    auto hand_mesh2 = scene.create_mesh("hand2");
    hand_mesh2->shared_color(svt::Colors::Blue);
    hand_mesh2->add_mesh(hand_hi2);

    test::assert_equal(scene.to_json(), "loop_subdivision_stencil", result);
    return result;
}