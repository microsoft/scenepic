#include "svt_tests.h"
#include "svt.h"

using svt::Scene;

int test_io()
{
    int result = EXIT_SUCCESS;
    svt::Scene scene;

    auto image = scene.create_image("texture");
    image->load(test::asset_path("PolarBear.png"));

    auto mesh = scene.create_mesh("cube");
    mesh->texture_id(image->image_id());
    auto mesh_info = svt::load_obj(test::asset_path("cube.obj"));
    mesh->add_mesh(mesh_info);

    test::assert_equal(mesh->to_json(), "io", result);

    return result;
}