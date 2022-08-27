"""Camera Movement example"""

import argparse
import os

import numpy as np
import scenepic as sp


Name = "camera_movement"
Title = "Camera Movement"


def asset_path(name: str) -> str:
    # returns the actual path to the asset
    return os.path.join("..", "ci", "assets", name)


def build_scene() -> sp.Scene:
    # One of the most important things in 3D visualization is camera control.
    # Scenepic allows you to set a per-frame camera and also to insert the
    # camera itself as an object into the scene.
    # while the user can always choose to override the default camera, having a
    # camera track specified can be helpful for demonstrating particular
    # items in 3D. We will also show off the flexible Camera class.

    scene = sp.Scene()
    spin_canvas = scene.create_canvas_3d("spin")
    spiral_canvas = scene.create_canvas_3d("spiral")

    # let's create some items in the scene so we have a frame of reference
    polar_bear = scene.create_image(image_id="polar_bear")
    polar_bear.load(asset_path("PolarBear.png"))
    uv_texture = scene.create_image(image_id="texture")
    uv_texture.load(asset_path("uv.png"))

    cube = scene.create_mesh("cube", texture_id=polar_bear.image_id)
    cube.add_cube()
    sphere = scene.create_mesh("sphere", texture_id=uv_texture.image_id)
    sphere.add_icosphere(steps=4, transform=sp.Transforms.translate([0, 1, 0]))

    num_frames = 60
    for i in range(num_frames):
        angle = i * np.pi * 2 / num_frames

        # for the first camera we will spin in place on the Z axis
        rotation = sp.Transforms.rotation_about_z(angle)
        spin_camera = sp.Camera(center=[0, 0, 4], rotation=rotation, fov_y_degrees=30.0)

        # for the second camera, we will spin the camera in a spiral around the scene
        # we can do this using the look-at initialization, which provides a straightforward
        # "look at" interface for camera placement.
        camera_center = [4 * np.cos(angle), i * 4 / num_frames - 2, 4 * np.sin(angle)]
        spiral_camera = sp.Camera(camera_center, look_at=[0, 0.5, 0])

        # we can add frustums directly using the ScenePic camera objects
        frustums = scene.create_mesh()
        frustums.add_camera_frustum(spin_camera, sp.Colors.Red)
        frustums.add_camera_frustum(spiral_camera, sp.Colors.Green)

        spin_frame = spin_canvas.create_frame()
        spin_frame.camera = spin_camera  # each frame can have its own camera object
        spin_frame.add_meshes([cube, sphere, frustums])

        spiral_frame = spiral_canvas.create_frame()
        spiral_frame.camera = spiral_camera
        spiral_frame.add_meshes([cube, sphere, frustums])

    scene.link_canvas_events(spin_canvas, spiral_canvas)
    return scene


def _parse_args():
    parser = argparse.ArgumentParser(Title)
    parser.add_argument("--script", action="store_true",
                        help="Whether to save the scenepic as a JS file")
    return parser.parse_args()


def _main():
    args = _parse_args()
    scene = build_scene()
    # The scene is complete, so we write it to a standalone file.
    if args.script:
        # If you have an existing HTML page you want to add a scenepic
        # to, then you can save the scenepic as a self-contained
        # Javascript file.
        scene.save_as_script("{}.js".format(Name), standalone=True)
    else:
        # However, ScenePic will also create a basic HTML wrapper
        # and embed the Javascript into the file directly so you
        # have a single file containing everything.
        scene.save_as_html("{}.html".format(Name), title=Title)


if __name__ == "__main__":
    _main()
