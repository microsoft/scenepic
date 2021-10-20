import numpy as np
import scenepic as sp


def _create_cube(scene: sp.Scene, color: np.ndarray) -> sp.Mesh:
    cube = scene.create_mesh(shared_color=color, layer_id="cubes")
    cube.add_cube(transform=sp.Transforms.scale(0.5))
    return cube


def _create_camera(angles: np.ndarray, distance: float,
                   focus_point: np.ndarray, aspect_ratio: float) -> sp.Camera:
    rotation = sp.Transforms.euler_angles_to_matrix(angles)

    # create a vector at the current distance
    pos = np.array([0, 0, distance], np.float32)

    # rotate and offset from the focus point
    pos = rotation[:3, :3] @ pos
    pos += focus_point

    return sp.Camera(pos, look_at=focus_point, aspect_ratio=aspect_ratio)


def _main():
    width = 512
    height = 200
    colors = [sp.ColorFromBytes(242, 80, 34),
              sp.ColorFromBytes(127, 186, 0),
              sp.ColorFromBytes(0, 164, 239),
              sp.ColorFromBytes(255, 185, 0)]

    scene = sp.Scene()
    canvas = scene.create_canvas_3d(html_id="logo-scenepic", width=width, height=height)
    canvas.shading = sp.Shading([0, 0, 0, 0])
    canvas.ui_parameters = sp.UIParameters(layer_dropdown_visibility="hidden")

    # load the text meshes
    scene_obj = sp.load_obj("scene.obj")
    scene_mesh = scene.create_mesh(shared_color=colors[0], layer_id="text")
    scene_mesh.add_mesh(scene_obj)
    scene_mesh.apply_transform(sp.Transforms.rotation_about_x(np.pi / 2))
    scene_pos = [0, 0, 0]

    pic_obj = sp.load_obj("pic.obj")
    pic_mesh = scene.create_mesh(shared_color=colors[3], layer_id="text")
    pic_mesh.add_mesh(pic_obj)
    pic_pos = [1.25, -0.05, 0.5]

    # create the cubes for morphing
    cubes = [_create_cube(scene, color) for color in colors]
    cube1_pos = [2, 0.15, 0.25]
    cube2_pos = [0.4, -0.05, 0.3]
    cube2_scale = [1.5, 0.2, 0.8]
    focus_point = [1, 0.25, 0.5]

    # this creates the ScenePic logo
    meshes = [scene_mesh, cubes[1], cubes[2], pic_mesh]
    transforms = [
        sp.Transforms.translate(scene_pos),
        sp.Transforms.translate(cube1_pos),
        sp.Transforms.translate(cube2_pos) @ sp.Transforms.scale(cube2_scale),
        sp.Transforms.translate(pic_pos)
    ]

    frame = canvas.create_frame()

    for mesh, transform in zip(meshes, transforms):
        frame.add_mesh(mesh, transform=transform)

    frame.camera = _create_camera([(-0.8, -0.2, 0)], 1.2,
                                  focus_point, width / height)
    frame.focus_point = sp.FocusPoint(focus_point)
    scene.save_as_script("logo.js", True)


if __name__ == "__main__":
    _main()
