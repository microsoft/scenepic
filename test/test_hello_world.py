"""Integration test taken from the Hello World example."""

import math

import numpy as np
import svt


def create_image_mesh(scene: svt.Scene, filename: str, layer_id: str):
    """Utility function to create an image mesh.

    Args:
        scene (svt.Scene): current Scene
        filename (str): path to the image
        layer_id (str): layer for the mesh

    Returns:
        Mesh: the image mesh
    """
    image = scene.create_image()
    image.load(filename)
    mesh = scene.create_mesh(texture_id=image.image_id, layer_id=layer_id, nn_texture=False)
    mesh.add_image(x_axis=np.array([[2.0, 0.0, 0.0]]))
    return mesh


SIZE = 500
NUM_FRAMES = 30


def test_hello_world(asset):
    """Integration test created from the hello world example."""
    scene = svt.Scene()

    mesh_hello = create_image_mesh(scene, asset("Hello.png"), "Hello")
    mesh_analog = create_image_mesh(scene, asset("Analog.png"), "Analog Science")
    mesh_science = create_image_mesh(scene, asset("Science.png"), "Analog Science")

    audio_hello = scene.create_audio("hello.mp3")

    hand_mesh = svt.load_obj(asset("hand.obj"))
    cmvs, cmts = svt.LoopSubdivStencil(hand_mesh.triangles, 1, True).apply(hand_mesh.positions)

    model_mesh_lo = scene.create_mesh(shared_color=svt.Color(1.0, 0.0, 0.0))
    model_mesh_lo.add_mesh(hand_mesh, reverse_triangle_order=True)
    model_mesh_hi = scene.create_mesh(shared_color=svt.Color(0.0, 0.0, 1.0))
    model_mesh_hi.add_mesh_without_normals(cmvs, cmts, reverse_triangle_order=True)

    canvas_hello = scene.create_canvas_3d(width=SIZE, height=SIZE, media_id=audio_hello.audio_id)
    hand_center = np.mean(hand_mesh.positions, axis=0)
    hand_camera = svt.Camera(center=hand_center + np.array([0.0, 0.0, 0.5]),
                             look_at=hand_center,
                             up_dir=np.array([0.0, 1.0, 0.0]),
                             fov_y_degrees=45.0)
    hand_shading = svt.Shading(bg_color=svt.Colors.White)
    hand_ui_parameters = svt.UIParameters()
    canvas_hand = scene.create_canvas_3d(width=SIZE,
                                         height=SIZE,
                                         camera=hand_camera,
                                         shading=hand_shading,
                                         ui_parameters=hand_ui_parameters)

    for i in range(NUM_FRAMES):
        angle = 2 * math.pi * i / NUM_FRAMES

        frame_hello = canvas_hello.create_frame()
        frame_hello.add_mesh(mesh_hello,
                             transform=svt.Transforms.Translate([math.cos(angle),
                                                                 math.sin(angle),
                                                                 0]))
        frame_hello.add_mesh(mesh_analog,
                             transform=svt.Transforms.Translate([math.cos(angle - 0.5 * math.pi),
                                                                 math.sin(angle - 0.5 * math.pi),
                                                                 -0.5]))
        frame_hello.add_mesh(mesh_science,
                             transform=svt.Transforms.Translate([math.cos(angle + math.pi),
                                                                 math.sin(angle + math.pi),
                                                                 -1.0]))

        mesh_primitives = scene.create_mesh(layer_id="Primitives")
        mesh_primitives.add_sphere(color=svt.Color(0, 1, 0),
                                   transform=svt.Transforms.Scale(0.2 * (2 + math.cos(angle))),
                                   fill_triangles=False,
                                   add_wireframe=True)
        mesh_primitives.add_cube(color=svt.Color(0, 0, 1),
                                 transform=svt.Transforms.Translate([-1.0, -1.0, -3.0]))
        frame_hello.add_mesh(mesh_primitives)

        mesh_noise = scene.create_mesh(shared_color=svt.Color(1.0, 0.0, 0.0), layer_id="Noise")
        mesh_noise.add_cylinder()
        mesh_noise.apply_transform(svt.Transforms.Scale([0.02, 0.1, 0.1]))
        mesh_noise.apply_transform(
            svt.Transforms.RotationToRotateXAxisToAlignWithAxis(np.array([0.5, 0.5, 0.5])))
        mesh_noise.enable_instancing(2 * np.random.rand(1000, 3) - 1)
        frame_hello.add_mesh(mesh_noise)

        frame_hand = canvas_hand.create_frame(focus_point=hand_center)
        frame_hand.add_mesh(model_mesh_lo,
                            transform=svt.Transforms.RotationAboutZAxis(math.cos(angle)))
        frame_hand.add_mesh(model_mesh_hi,
                            transform=svt.Transforms.RotationAboutZAxis(math.cos(angle + math.pi)))

    canvas_hello.set_layer_settings({
        "Hello" : {"filled" : True},
        "Analog Science" : {"filled" : True},
        "Primitives" : {"filled" : False},
        "Noise" : {"filled" : False, "opacity" : 0.5}})
