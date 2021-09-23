"""Tests taken from the Sky Box example."""

import math

import numpy as np
import svt


QUARTER = 1.0 / 4.0
THIRD = 1.0 / 3.0
DEC = 1.0 / 1000


def test_skybox(asset):
    """Test creating a skybox."""
    scene = svt.Scene()
    canvas = scene.create_canvas_3d(width=500, height=500)

    image = scene.create_image(image_id="SkyBox")
    image.load(asset("SkyBox.jpg"))

    skybox = scene.create_mesh(shared_color=svt.Colors.Blue, texture_id="SkyBox")
    skybox.add_image(origin=np.array([[-2.5, -0.5, -0.5]]), double_sided=False)
    skybox.add_image(origin=np.array([[-0.5, -0.5, -0.5]]),
                     double_sided=False,
                     uv_0=np.array([[QUARTER + DEC, THIRD + DEC]]),
                     uv_1=np.array([[2 * QUARTER - DEC, THIRD + DEC]]),
                     uv_2=np.array([[2 * QUARTER - DEC, 2 * THIRD - DEC]]),
                     uv_3=np.array([[QUARTER + DEC, 2 * THIRD - DEC]]))
    skybox.add_image(origin=np.array([[-0.5, -0.5, -0.5]]),
                     double_sided=False,
                     uv_0=np.array([[0 + DEC, THIRD + DEC]]),
                     uv_1=np.array([[QUARTER - DEC, THIRD + DEC]]),
                     uv_2=np.array([[QUARTER - DEC, 2 * THIRD - DEC]]),
                     uv_3=np.array([[0 + DEC, 2 * THIRD - DEC]]),
                     transform=svt.Transforms.RotationAboutYAxis(math.pi / 2.0))
    skybox.add_image(origin=np.array([[-0.5, -0.5, -0.5]]),
                     double_sided=False,
                     uv_0=np.array([[2 * QUARTER + DEC, THIRD + DEC]]),
                     uv_1=np.array([[3 * QUARTER - DEC, THIRD + DEC]]),
                     uv_2=np.array([[3 * QUARTER - DEC, 2 * THIRD - DEC]]),
                     uv_3=np.array([[2 * QUARTER + DEC, 2 * THIRD - DEC]]),
                     transform=svt.Transforms.RotationAboutYAxis(-math.pi / 2.0))
    skybox.add_image(origin=np.array([[-0.5, -0.5, -0.5]]),
                     double_sided=False,
                     uv_0=np.array([[3 * QUARTER + DEC, THIRD + DEC]]),
                     uv_1=np.array([[4 * QUARTER - DEC, THIRD + DEC]]),
                     uv_2=np.array([[4 * QUARTER - DEC, 2 * THIRD - DEC]]),
                     uv_3=np.array([[3 * QUARTER + DEC, 2 * THIRD - DEC]]),
                     transform=svt.Transforms.RotationAboutYAxis(math.pi))
    skybox.add_image(origin=np.array([[-0.5, -0.5, -0.5]]),
                     double_sided=False,
                     uv_0=np.array([[QUARTER + DEC, 2 * THIRD + DEC]]),
                     uv_1=np.array([[2 * QUARTER - DEC, 2 * THIRD + DEC]]),
                     uv_2=np.array([[2 * QUARTER - DEC, 3 * THIRD - DEC]]),
                     uv_3=np.array([[QUARTER + DEC, 3 * THIRD - DEC]]),
                     transform=svt.Transforms.RotationAboutXAxis(math.pi / 2.0))
    skybox.add_image(origin=np.array([[-0.5, -0.5, -0.5]]),
                     double_sided=False,
                     uv_0=np.array([[QUARTER + DEC, 0 + DEC]]),
                     uv_1=np.array([[2 * QUARTER - DEC, 0 + DEC]]),
                     uv_2=np.array([[2 * QUARTER - DEC, THIRD - DEC]]),
                     uv_3=np.array([[QUARTER + DEC, THIRD - DEC]]),
                     transform=svt.Transforms.RotationAboutXAxis(-math.pi / 2.0))

    geometry = scene.create_mesh()
    geometry.add_cube(color=svt.Colors.Blue,
                      add_wireframe=True,
                      fill_triangles=False)
    geometry.apply_transform(svt.Transforms.Scale(0.995))

    transform = np.dot(svt.Transforms.Scale(30.0), svt.Transforms.Translate([0, 0.5, 0]))
    skybox.apply_transform(transform)
    geometry.apply_transform(transform)

    frame = canvas.create_frame()
    frame.add_mesh(skybox)
    frame.add_mesh(geometry)


HOLO_DECK_WIDTH = 12.5
SPACE_BETWEEN_LINES = 1.25
COUNT_LINES = int(HOLO_DECK_WIDTH / (2 * SPACE_BETWEEN_LINES)) * 2 + 1
LINE_WIDTH = 0.0025
LINE_COLOR = svt.ColorFromBytes(255, 235, 100)


def test_holodeck(asset):
    """Test creating a holodeck."""
    scene = svt.Scene()
    canvas = scene.create_canvas_3d(width=500, height=500)

    wall = svt.Mesh(shared_color=LINE_COLOR)
    for line in range(COUNT_LINES + 1):
        x = line / COUNT_LINES - 0.5
        wall.add_quad(p0=np.array([[x - LINE_WIDTH, -0.5, -0.5]]),
                      p1=np.array([[x + LINE_WIDTH, -0.5, -0.5]]),
                      p2=np.array([[x + LINE_WIDTH, +0.5, -0.5]]),
                      p3=np.array([[x - LINE_WIDTH, +0.5, -0.5]]))
        wall.add_quad(p0=np.array([[-0.5, x - LINE_WIDTH, -0.5]]),
                      p3=np.array([[-0.5, x + LINE_WIDTH, -0.5]]),
                      p2=np.array([[+0.5, x + LINE_WIDTH, -0.5]]),
                      p1=np.array([[+0.5, x - LINE_WIDTH, -0.5]]))

    holodeck = scene.create_mesh(mesh_id="HoloDeckWebVRBackground",
                                 shared_color=LINE_COLOR)
    holodeck.append_mesh(wall)
    holodeck.append_mesh(wall.get_transformed(
        svt.Transforms.RotationAboutXAxis(angle=math.pi)))
    holodeck.append_mesh(wall.get_transformed(
        svt.Transforms.RotationAboutYAxis(angle=-0.5 * math.pi)))
    holodeck.append_mesh(wall.get_transformed(
        svt.Transforms.RotationAboutYAxis(angle=+0.5 * math.pi)))
    holodeck.append_mesh(wall.get_transformed(
        svt.Transforms.RotationAboutXAxis(angle=-0.5 * math.pi)))
    holodeck.append_mesh(wall.get_transformed(
        svt.Transforms.RotationAboutXAxis(angle=+0.5 * math.pi)))
    holodeck.apply_transform(svt.Transforms.Scale(HOLO_DECK_WIDTH))

    ground = scene.create_image(image_id="HoloDeckWebVRGroundPlaneTexture")
    ground.load(asset("Ground.jpg"))
    floor = scene.create_mesh(mesh_id="HoloDeckWebVRGroundPlane",
                              shared_color=svt.Colors.Grey,
                              texture_id=ground,
                              nn_texture=False)
    floor.add_image(origin=np.array([[+0.25, 0, -0.25]]),
                    x_axis=np.array([[-0.5, 0, 0]]),
                    y_axis=np.array([[0, +0, +0.5]]),
                    uv_0=np.array([[0.0, 0.0]]),
                    uv_1=np.array([[4.0, 0.0]]),
                    uv_2=np.array([[4.0, 4.0]]),
                    uv_3=np.array([[0.0, 4.0]]),
                    double_sided=False)
    floor.apply_transform(svt.Transforms.Scale(HOLO_DECK_WIDTH))

    frame = canvas.create_frame()
    frame.add_mesh(holodeck)
    frame.add_mesh(floor)
