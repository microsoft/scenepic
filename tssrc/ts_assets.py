"""Script to create the holodeck assets."""

import os

import numpy as np
import svt


def _focuspoint_mesh():
    scene = svt.Scene()
    mesh = scene.create_mesh(shared_color=svt.Color(1.0, 0.0, 1.0))
    mesh.add_cube()
    with open("focus_point.json", "w") as file:
        file.write(str(mesh))


def _webvr_background(holo_deck_width=12.5,
                      space_between_lines=1.25,
                      line_width=0.0025,
                      line_color=None):
    if line_color is None:
        line_color = svt.ColorFromBytes(255, 236, 100)

    num_lines = int(holo_deck_width / (2 * space_between_lines)) * 2 + 1  # Ensure an odd number

    # Create scene/canvas
    scene = svt.Scene()
    canvas = scene.create_canvas_3d(width=500, height=500)

    # Create back wall
    wall = svt.Mesh(shared_color=line_color)
    for line in range(num_lines + 1):
        p = float(line) / num_lines - 0.5
        wall.add_quad(p0=np.array([[p - line_width, -0.5, -0.5]]),
                      p1=np.array([[p + line_width, -0.5, -0.5]]),
                      p2=np.array([[p + line_width, +0.5, -0.5]]),
                      p3=np.array([[p - line_width, +0.5, -0.5]]))
        wall.add_quad(p0=np.array([[-0.5, p - line_width, -0.5]]),
                      p3=np.array([[-0.5, p + line_width, -0.5]]),
                      p2=np.array([[+0.5, p + line_width, -0.5]]),
                      p1=np.array([[+0.5, p - line_width, -0.5]]))

    # Create holodeck
    holodeck = scene.create_mesh(mesh_id="HoloDeckWebVRBackground", shared_color=line_color)
    holodeck.append_mesh(wall)
    holodeck.append_mesh(wall.get_transformed(svt.Transforms.RotationAboutXAxis(angle=np.pi)))
    holodeck.append_mesh(wall.get_transformed(svt.Transforms.RotationAboutYAxis(angle=-0.5*np.pi)))
    holodeck.append_mesh(wall.get_transformed(svt.Transforms.RotationAboutYAxis(angle=+0.5*np.pi)))
    holodeck.append_mesh(wall.get_transformed(svt.Transforms.RotationAboutXAxis(angle=-0.5*np.pi)))
    holodeck.append_mesh(wall.get_transformed(svt.Transforms.RotationAboutXAxis(angle=+0.5*np.pi)))
    holodeck.apply_transform(svt.Transforms.Scale(holo_deck_width))

    # Add platform to stand on
    ground = scene.create_image(image_id="HoloDeckWebVRGroundPlaneTexture")
    ground.load(os.path.join("..", "examples", "SkyBox", "Ground.jpg"))
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

    floor.apply_transform(svt.Transforms.Scale(holo_deck_width))

    # Create frame
    frame = canvas.create_frame()
    frame.add_mesh(holodeck)
    frame.add_mesh(floor)

    scene.save_as_json("webvr_background.json")


if __name__ == "__main__":
    _focuspoint_mesh()
    _webvr_background()
