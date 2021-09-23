#include "svt_tests.h"
#include "scenepic.h"

using svt::Scene;

int test_mesh_update()
{
    int result = EXIT_SUCCESS;
    svt::Scene scene;

    auto mesh = scene.create_mesh("base");
    mesh->add_triangle(test::COLOR);

    svt::VectorBuffer positions(3, 3);
    positions << 0, 0, 0,
                 1, 0, 0,
                 0, 0, 1;

    svt::VectorBuffer normals(3, 3);
    normals << 0, -1, 0,
               0, -1, 0,
               0, -1, 0;

    auto update = scene.update_mesh("base", positions, "update0");
    
    test::assert_equal(update->to_json(), "update0", result);

    update = scene.update_mesh("base", positions, normals, "update1");

    test::assert_equal(update->to_json(), "update1", result);

    svt::VertexBuffer keyframe_buffer = update->vertex_buffer();
    keyframe_buffer.topLeftCorner(1, 3) << 0, 1, 1;

    update->quantize(1, 6.0f, keyframe_buffer);

    test::assert_equal(update->to_json(), "update_quantized", result);

    return result;
}