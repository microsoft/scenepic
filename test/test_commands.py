"""Tests for command serialization."""

# Copyright (c) Microsoft Corporation.
# Licensed under the MIT License.

import numpy as np
from PIL import Image
import pytest
import scenepic as sp


def test_color():
    expected = sp.Color(0.5, 0, 0.2)
    actual = sp.ColorFromBytes(255, 0, 102) * 0.5
    np.testing.assert_array_almost_equal(actual, expected)


def test_camera(assert_json_equal):
    center = np.array([0, 2, 0], np.float32)
    look_at = np.array([0, 1, 0], np.float32)
    up_dir = np.array([1, 0, 0], np.float32)
    fov_y_degrees = 45.0
    initial_aspect_ratio = 1.5
    new_aspect_ratio = 0.9
    znear = 0.01
    zfar = 20

    rotation = np.array([
        [0, 0, 1, 0],
        [1, 0, 0, 0],
        [0, 1, 0, 0],
        [0, 0, 0, 1]
    ], np.float32)

    world_to_camera = np.array([
        [0, 0, 1, 0],
        [1, 0, 0, 0],
        [0, 1, 0, -2],
        [0, 0, 0, 1]
    ], np.float32)

    camera_to_world = np.array([
        [0, 1, 0, 0],
        [0, 0, 1, 2],
        [1, 0, 0, 0],
        [0, 0, 0, 1]
    ], np.float32)

    projection = sp.Transforms.gl_projection(fov_y_degrees, initial_aspect_ratio, znear, zfar)

    look_at_cam = sp.Camera(center, look_at, up_dir, fov_y_degrees, znear, zfar, initial_aspect_ratio)
    np.testing.assert_array_almost_equal(look_at_cam.center, center)
    np.testing.assert_array_almost_equal(look_at_cam.look_at, look_at)
    np.testing.assert_array_almost_equal(look_at_cam.up_dir, up_dir)
    np.testing.assert_array_almost_equal(look_at_cam.world_to_camera, world_to_camera)
    np.testing.assert_array_almost_equal(look_at_cam.camera_to_world, camera_to_world)
    np.testing.assert_array_almost_equal(look_at_cam.projection, projection)
    assert_json_equal(str(look_at_cam), "camera")

    look_at_cam.aspect_ratio = new_aspect_ratio
    assert look_at_cam.aspect_ratio == pytest.approx(new_aspect_ratio)

    rt_cam = sp.Camera(center, rotation, fov_y_degrees, znear, zfar, new_aspect_ratio)
    np.testing.assert_array_almost_equal(rt_cam.center, center)
    np.testing.assert_array_almost_equal(rt_cam.look_at, look_at)
    np.testing.assert_array_almost_equal(rt_cam.up_dir, up_dir)
    np.testing.assert_array_almost_equal(rt_cam.world_to_camera, world_to_camera)
    np.testing.assert_array_almost_equal(rt_cam.camera_to_world, camera_to_world)
    assert rt_cam.aspect_ratio == pytest.approx(new_aspect_ratio)
    rt_cam.aspect_ratio = initial_aspect_ratio
    np.testing.assert_array_almost_equal(rt_cam.projection, projection)
    assert_json_equal(str(rt_cam), "camera")

    cam = sp.Camera(world_to_camera, fov_y_degrees, znear, zfar, new_aspect_ratio)
    np.testing.assert_array_almost_equal(cam.center, center)
    np.testing.assert_array_almost_equal(cam.look_at, look_at)
    np.testing.assert_array_almost_equal(cam.up_dir, up_dir)
    np.testing.assert_array_almost_equal(cam.world_to_camera, world_to_camera)
    np.testing.assert_array_almost_equal(cam.camera_to_world, camera_to_world)
    assert cam.aspect_ratio == pytest.approx(new_aspect_ratio)
    cam.aspect_ratio = initial_aspect_ratio
    np.testing.assert_array_almost_equal(cam.projection, projection)
    assert_json_equal(str(cam), "camera")


def test_canvas2d(assert_json_equal):
    scene = sp.Scene()
    canvas2d = scene.create_canvas_2d("canvas2d")
    frame2d = canvas2d.create_frame()
    frame2d.add_circle(0, 0, 5)

    frame2d = canvas2d.create_frame()
    frame2d.add_rectangle(5, 6, 7, 8)

    frame2d = canvas2d.create_frame()
    frame2d.add_text("test", 1, 1)

    frame2d = canvas2d.create_frame()
    frame2d.add_image("rand")

    assert_json_equal(str(canvas2d), "canvas2d")
    canvas2d.clear_script()
    frame2d = canvas2d.create_frame()
    positions = np.array(
        [[0, 0],
         [1, 1],
         [2, 2]], np.float32
    )
    frame2d.add_line(positions)

    assert_json_equal(str(canvas2d), "canvas2d_cleared")


def test_canvas3d(assert_json_equal, color):
    scene = sp.Scene()

    cube_mesh = scene.create_mesh("cube")
    cube_mesh.add_cube(color)

    cone_mesh = scene.create_mesh("cone")
    cone_mesh.add_cone(color)

    disc_mesh = scene.create_mesh("disc")
    disc_mesh.add_disc(color)

    icosphere_mesh = scene.create_mesh("icosphere")
    icosphere_mesh.add_icosphere(color)

    cylinder_mesh = scene.create_mesh("cylinder")
    cylinder_mesh.add_cylinder(color)

    canvas3d = scene.create_canvas_3d("canvas3d")
    frame3d = canvas3d.create_frame("", [1, 0, 0])
    frame3d.add_mesh(cube_mesh)

    frame3d = canvas3d.create_frame()
    frame3d.add_mesh(disc_mesh, sp.Transforms.Scale(5))

    frame3d = canvas3d.create_frame()
    frame3d.add_mesh(icosphere_mesh)

    frame3d = canvas3d.create_frame()
    frame3d.add_mesh(cylinder_mesh)

    assert_json_equal(str(canvas3d), "canvas3d")

    canvas3d.clear_script()
    frame3d = canvas3d.create_frame()
    frame3d.add_mesh(cone_mesh)

    assert_json_equal(str(canvas3d), "canvas3d_cleared")


def test_drop_down_menu(assert_json_equal):
    scene = sp.Scene()
    drop_down_menu = scene.create_drop_down_menu("", "DropDown")
    drop_down_menu.items = ["one", "two", "three"]
    assert_json_equal(str(drop_down_menu), "drop_down_menu")


def test_frame2d(assert_json_equal):
    scene = sp.Scene()
    canvas2d = scene.create_canvas_2d()
    frame2d = canvas2d.create_frame()
    frame2d.add_circle(0, 0, 5.0)
    assert_json_equal(str(frame2d), "frame2d")


def test_frame3d(assert_json_equal, color):
    scene = sp.Scene()
    cube_mesh = scene.create_mesh("cube")
    cube_mesh.add_cube(color)

    canvas3d = scene.create_canvas_3d()
    frame3d = canvas3d.create_frame("", [1, 0, 0])
    frame3d.add_mesh(cube_mesh)

    assert_json_equal(str(frame3d), "frame3d")


def test_image(assert_json_equal, color, asset):
    scene = sp.Scene()
    image = scene.create_image("rand")
    image.load(asset("rand.png"))
    assert_json_equal(str(image), "image")

    pixels = np.array(Image.open(asset("rand.png")))
    image.from_numpy(pixels, "png")
    assert_json_equal(str(image), "image")

    with open(asset("rand.png"), "rb") as file:
        buffer = file.read()

    image.load_from_buffer(buffer, "png")
    assert_json_equal(str(image), "image")

    mesh = scene.create_mesh("image")
    mesh.texture_id = image.image_id
    mesh.add_image()
    assert_json_equal(str(mesh), "image_mesh")


def test_audio(assert_json_equal, asset):
    scene = sp.Scene()
    audio = scene.create_audio("hello")
    audio.load(asset("hello.mp3"))
    assert_json_equal(str(audio), "audio")


def test_video(assert_json_equal, asset):
    scene = sp.Scene()
    video = scene.create_video("test")
    video.load(asset("test.mp4"))
    assert_json_equal(str(video), "video")


def test_label(assert_json_equal):
    scene = sp.Scene()
    scene.create_label()
    assert_json_equal(scene.get_json(), "label")


def test_primitives(assert_json_equal, asset, color):
    scene = sp.Scene()
    texture = scene.create_image("uv")
    texture.load(asset("uv.png"))

    mesh = scene.create_mesh("triangle")
    mesh.add_triangle(color)
    assert_json_equal(str(mesh), "triangle")

    mesh = scene.create_mesh("quad")
    mesh.add_quad(color)
    assert_json_equal(str(mesh), "quad")

    mesh = scene.create_mesh("cube")
    mesh.add_cube(color)
    assert_json_equal(str(mesh), "cube")

    mesh = scene.create_mesh("cube_texture", texture_id=texture.image_id)
    mesh.add_cube()
    assert_json_equal(str(mesh), "cube_texture")

    mesh = scene.create_mesh("thickline")
    mesh.add_thickline(color)
    assert_json_equal(str(mesh), "thickline")

    mesh = scene.create_mesh("cone")
    mesh.add_cone(color)
    assert_json_equal(str(mesh), "cone")

    mesh = scene.create_mesh("trunc_cone")
    mesh.add_cone(color, truncation_height=0.7)
    assert_json_equal(str(mesh), "trunc_cone")

    mesh = scene.create_mesh("coordinate_axes_0")
    mesh.add_coordinate_axes()
    assert_json_equal(str(mesh), "coordinate_axes_0")

    mesh = scene.create_mesh("coordinate_axes_1", shared_color=sp.Colors.White)
    mesh.add_cube()
    mesh.add_coordinate_axes()
    assert_json_equal(str(mesh), "coordinate_axes_1")

    mesh = scene.create_mesh("camera_frustum")
    mesh.add_camera_frustum(color)
    assert_json_equal(str(mesh), "camera_frustum")

    mesh = scene.create_mesh("disc")
    mesh.add_disc(color)
    assert_json_equal(str(mesh), "disc")

    mesh = scene.create_mesh("cylinder")
    mesh.add_cylinder(color)
    assert_json_equal(str(mesh), "cylinder")

    mesh = scene.create_mesh("sphere")
    mesh.add_sphere(color)
    assert_json_equal(str(mesh), "sphere")

    mesh = scene.create_mesh("icosphere")
    mesh.add_icosphere(color)
    assert_json_equal(str(mesh), "icosphere")

    mesh = scene.create_mesh("icosphere_texture", texture_id=texture.image_id)
    mesh.add_icosphere(steps=1)
    assert_json_equal(str(mesh), "icosphere_texture")

    mesh = scene.create_mesh("uv_sphere")
    mesh.add_uv_sphere(color)
    assert_json_equal(str(mesh), "uv_sphere")

    mesh = scene.create_mesh("point_cloud")
    positions = []
    for x in range(5):
        for y in range(5):
            for z in range(5):
                positions.append([x, y, z])
    positions = np.array(positions, np.float32)
    positions = (positions / 2) - 1
    mesh.add_cube(color)
    mesh.apply_transform(sp.Transforms.scale(0.01))
    mesh.enable_instancing(positions)
    assert_json_equal(str(mesh), "point_cloud")

    mesh = scene.create_mesh("line_cloud")
    mesh.add_lines(positions, positions * 10, color)
    assert_json_equal(str(mesh), "line_cloud")


def test_io(assert_json_equal, asset):
    scene = sp.Scene()

    image = scene.create_image("texture")
    image.load(asset("PolarBear.png"))

    mesh = scene.create_mesh("cube")
    mesh.texture_id = image.image_id

    mesh_info = sp.load_obj(asset("cube.obj"))
    mesh.add_mesh(mesh_info)

    assert_json_equal(str(mesh), "io")


def test_mesh_update(assert_json_equal, color):
    scene = sp.Scene()

    mesh = scene.create_mesh("base")
    mesh.add_triangle(color)

    positions = np.array([
        [0, 0, 0],
        [1, 0, 0],
        [0, 0, 1]
    ], np.float32)

    normals = np.array([
        [0, -1, 0],
        [0, -1, 0],
        [0, -1, 0]
    ], np.float32)

    colors = np.array([
        [1, 0, 0],
        [0, 1, 0],
        [0, 0, 1]
    ], np.float32)

    update = scene.update_mesh_positions("base", positions, "update0")

    assert_json_equal(str(update), "update0")

    update = scene.update_mesh("base", positions, normals, colors, "update1")

    assert_json_equal(str(update), "update1")

    keyframe_buffer = update.vertex_buffer.copy()
    keyframe_buffer["pos"][0] = [0, 1, 1]

    update.quantize(1, 6.0, keyframe_buffer)

    assert_json_equal(str(update), "update_quantized")

    instance_pos = np.array([
        [0, 1, 2],
        [2, 0, 1],
        [1, 0, 2]
    ], np.float32)

    instance_rot = np.array([
        [0.11, 0.22, 0.46, 0.85],
        [0.46, -0.12, -0.22, 0.85],
        [0.22, -0.12, 0.46, 0.85]
    ], np.float32)

    instance_c = np.array([
        [1, 0, 0],
        [0, 1, 0],
        [0, 0, 1]
    ], np.float32)

    mesh.enable_instancing(instance_pos, instance_rot, instance_c)

    instance_pos[0] = [1, 1, 0]

    update = scene.update_mesh_positions("base", instance_pos, "update2")

    assert_json_equal(str(update), "update2")

    instance_pos[1] = [1, 0, 1]
    instance_rot[0] = [0.24, 0.24, 0.06, 0.94]
    instance_c[0] = [0.5, 0, 0]

    update = scene.update_instanced_mesh("base", instance_pos, instance_rot, instance_c, "update3")

    assert_json_equal(str(update), "update3")


def test_quantization(assert_json_equal, color):
    scene = sp.Scene()

    mesh = scene.create_mesh("base")
    mesh.add_triangle(color)

    for i in range(20):
        positions = np.array([
            [0, 0, 0],
            [1, i * 0.05, 0],
            [0, 1, 0]
        ], np.float32)
        scene.update_mesh_positions("base", positions)

    info = scene.quantize_updates(1e-5)

    assert info["base"].keyframe_count == 2
    assert_json_equal(scene.get_json(), "quantization")


def _create_tetrahedron():
    vertices = np.array([
        [-0.5, -0.32476, -0.20412],
        [0.5, -0.32476, -0.20412],
        [0, 0.541266, -0.20412],
        [0, 0.108253, 0.612372]
    ], np.float32)

    triangles = np.array([
        [0, 1, 3],
        [1, 2, 3],
        [2, 0, 3],
        [0, 2, 1]
    ], np.uint32)

    return vertices, triangles


SIZE = 500


def test_scene(assert_json_equal, asset):
    scene = sp.Scene("test")
    image = scene.create_image("rand")
    image.load(asset("rand.png"))
    mesh_rand = scene.create_mesh()
    mesh_rand.texture_id = image.image_id
    mesh_rand.layer_id = "Test"
    mesh_rand.add_image([-0.5, -0.5, 0], [2, 0, 0])

    tet_verts, tet_tris = _create_tetrahedron()

    model_mesh = scene.create_mesh()
    model_mesh.shared_color = sp.Color(1, 0, 0)
    model_mesh.add_mesh_without_normals(tet_verts, tet_tris)
    model_mesh.reverse_triangle_order()

    canvas_rand = scene.create_canvas_3d("", SIZE, SIZE)
    tet_center = tet_verts.mean(axis=0)
    canvas_tet = scene.create_canvas_3d("", SIZE, SIZE)
    canvas_tet.camera = sp.Camera(tet_center + np.array([0, 0, 0.5], np.float32),
                                  tet_center, [0, 1, 0], 45.0)
    canvas_tet.shading = sp.Shading(sp.Colors.White)
    canvas_tet.ui_parameters = sp.UIParameters()

    n_frames = 5
    for i in range(n_frames):
        angle = 2 * np.pi * i / n_frames

        frame_rand = canvas_rand.create_frame()
        frame_rand.add_mesh(mesh_rand, sp.Transforms.Translate([np.cos(angle), np.sin(angle), 0]))

        mesh_primitives = scene.create_mesh()
        mesh_primitives.layer_id = "Primitives"
        mesh_primitives.add_disc(sp.Color(0, 1, 0),
                                 sp.Transforms.Scale(0.2 + 0.2 * (1 + np.cos(angle))),
                                 10, False, True)
        mesh_primitives.add_cube(sp.Color(0, 0, 1), sp.Transforms.Translate([-1, -1, -3]))
        frame_rand.add_mesh(mesh_primitives)

        mesh_noise = scene.create_mesh()
        mesh_noise.shared_color = sp.Color(1, 0, 0)
        mesh_noise.layer_id = "Noise"
        mesh_noise.add_cylinder()
        mesh_noise.apply_transform(sp.Transforms.Scale([0.02, 0.1, 0.1]))
        mesh_noise.apply_transform(sp.Transforms.RotationToRotateXAxisToAlignWithAxis([0.5, 0.5, 0.5]))
        positions = np.zeros((16, 3), np.float32)
        for j in range(16 * 3):
            value = (j % 4) / 4.0
            positions[j // 3, j % 3] = 2 * value - 1

        mesh_noise.enable_instancing(positions)
        frame_rand.add_mesh(mesh_noise)

        frame_tet = canvas_tet.create_frame("", tet_center)
        frame_tet.add_mesh(model_mesh, sp.Transforms.RotationAboutZAxis(np.cos(angle)))

    canvas_rand.set_layer_settings({
        "Test": {"filled": True},
        "Primitives": {"filled": False},
        "Noise": {"filled": False, "opacity": 0.5}
    })

    assert_json_equal(scene.get_json(), "scene")

    scene.clear_script()

    frame_tet = canvas_tet.create_frame("", tet_center)
    frame_tet.add_mesh(model_mesh, sp.Transforms.RotationAboutZAxis(np.cos(2 * np.pi)))

    assert_json_equal(scene.get_json(), "scene_cleared")


def test_layer_settings(assert_json_equal):
    scene = sp.Scene()
    canvas3d = scene.create_canvas_3d()
    canvas2d = scene.create_canvas_2d()

    mesh0 = scene.create_mesh("null")
    mesh1 = scene.create_mesh("one", layer_id="one")
    mesh2 = scene.create_mesh("two", layer_id="two")
    image0 = scene.create_image("null")
    image1 = scene.create_image("one")
    image2 = scene.create_image("two")
    video = scene.create_video()

    canvas2d.media_id = video.video_id

    for _ in range(2):
        frame = canvas3d.create_frame()
        frame.add_mesh(mesh0)
        frame.add_mesh(mesh1)
        frame.add_mesh(mesh2)

        frame = canvas2d.create_frame()
        frame.add_circle(0, 0, 1)
        frame.add_circle(0, 0, 2, layer_id="one")
        frame.add_circle(0, 0, 3, layer_id="two")

        frame.add_text("null", 0, 0)
        frame.add_text("one", 100, 100, layer_id="one")
        frame.add_text("two", 200, 200, layer_id="two")

        coords = np.zeros((2, 2), np.float32)
        frame.add_line(coords)
        frame.add_line(coords + 1, layer_id="one")
        frame.add_line(coords + 2, layer_id="two")

        frame.add_rectangle(0, 0, 0, 0)
        frame.add_rectangle(0, 0, 10, 10, layer_id="one")
        frame.add_rectangle(0, 0, 20, 20, layer_id="two")

        frame.add_image(image0)
        frame.add_image(image1, layer_id="one")
        frame.add_image(image2, layer_id="two")

        frame.add_video("manual", 0, 0)
        frame.add_video("manual", 1, 1, layer_id="one")
        frame.add_video("manual", 2, 2, layer_id="two")

    layer_settings = {
        "one": {"opacity": 0.5, "render_order": 1},
        "two": {"opacity": 0.9}
    }

    canvas2d.set_layer_settings(layer_settings)
    canvas3d.set_layer_settings(layer_settings)

    assert_json_equal(str(canvas2d), "layer_settings_2d")
    assert_json_equal(str(canvas3d), "layer_settings_3d")


def test_shading(assert_json_equal):
    shading = sp.Shading()
    assert_json_equal(str(shading), "shading")


def test_text_panel(assert_json_equal):
    scene = sp.Scene()
    text_panel = scene.create_text_panel()
    text_panel.text = "Text"
    text_panel.title = "Title"
    assert_json_equal(str(text_panel), "text_panel")


def test_ui_parameters(assert_json_equal):
    ui_parameters = sp.UIParameters()
    assert_json_equal(str(ui_parameters), "ui_parameters")


def test_graph(assert_json_equal):
    scene = sp.Scene()
    graph = scene.create_graph(name_align="top", value_align="bottom",
                               name_size=32, value_size=10)
    t = np.linspace(0, 1, 30)
    graph.add_sparkline("cos", np.cos(t), sp.Colors.Red,
                        vertical_rules=[
                            sp.Graph.VerticalRule(10, sp.Colors.Red, 2.0),
                            sp.Graph.VerticalRule(20, sp.Colors.Green)])
    graph.add_sparkline("sin", np.sin(t), line_width=2)
    assert_json_equal(str(graph), "graph")
