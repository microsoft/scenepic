"""Test the loop subdivision stencil."""

# Copyright (c) Microsoft Corporation.
# Licensed under the MIT License.

import scenepic as sp


def test_loop_subdivision_stencil(asset, assert_json_equal):
    hand_lo = sp.load_obj(asset("hand.obj"))

    hand_verts1, hand_tris1 = sp.LoopSubdivStencil(hand_lo.triangles,
                                                   steps=1,
                                                   project_to_limit=True).apply(hand_lo.positions)
    hand_hi2 = hand_lo.subdivide(2)

    scene = sp.Scene()
    hand_mesh1 = scene.create_mesh("hand1", shared_color=sp.Colors.Pink)
    hand_mesh1.add_mesh_without_normals(hand_verts1, hand_tris1)
    head_mesh2 = scene.create_mesh("hand2", shared_color=sp.Colors.Blue)
    head_mesh2.add_mesh(hand_hi2)

    assert_json_equal(scene.get_json(), "loop_subdivision_stencil")
