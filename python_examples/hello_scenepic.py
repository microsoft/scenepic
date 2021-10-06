import os

import numpy as np
import scenepic as sp


def _asset(name: str):
    asset_dir = os.path.join(os.path.dirname(__file__), "..", "ci", "assets")
    return os.path.join(asset_dir, name)


def _main():
    scene_obj = sp.load_obj(_asset("scene.obj"))
    pic_obj = sp.load_obj(_asset("pic.obj"))

    colors = [sp.ColorFromBytes(242, 80, 34),
              sp.ColorFromBytes(127, 186, 0),
              sp.ColorFromBytes(0, 164, 239),
              sp.ColorFromBytes(255, 185, 0)]

    scene = sp.Scene()
    canvas = scene.create_canvas_3d(width=1280, height=640)

    scene_mesh = scene.create_mesh(shared_color=colors[0])
    scene_mesh.add_mesh(scene_obj)

    pic_mesh = scene.create_mesh(shared_color=colors[3])
    pic_mesh.add_mesh(pic_obj)

    cube_mesh = scene.create_mesh(shared_color=colors[1])
    cube_mesh.add_cube(transform=sp.Transforms.scale(0.5))

    sphere_mesh = scene.create_mesh(shared_color=colors[2])
    sphere_mesh.add_cube(transform=sp.Transforms.scale([0.7, 0.1, 0.4]))

    # idea: start with one cube per color, then (for the text)
    # double the number per frame while reducing size until
    # each little cube is at its position and then swap in the
    # text

    frame = canvas.create_frame()
    frame.add_mesh(scene_mesh, sp.Transforms.rotation_about_x(np.pi / 2))
    frame.add_mesh(pic_mesh, sp.Transforms.translate([1.25, -0.05, 0.5]))
    frame.add_mesh(cube_mesh, sp.Transforms.translate([2, 0.15, 0.25]))
    frame.add_mesh(sphere_mesh, sp.Transforms.translate([0.35, -0.05, 0.3]))

    scene.save_as_html("hello_scenepic.html", "Hello Scenepic!")


if __name__ == "__main__":
    _main()
