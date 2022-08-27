"""Mesh Animation example"""

import argparse
import os

import numpy as np
import scenepic as sp


Name = "mesh_animation"
Title = "Mesh Animation"


def asset_path(name: str) -> str:
    # returns the actual path to the asset
    return os.path.join("..", "ci", "assets", name)


def build_scene() -> sp.Scene:
    # let's create our mesh to get started
    scene = sp.Scene()
    canvas = scene.create_canvas_3d(width=700, height=700)

    # Load a mesh to animate
    jelly_mesh = sp.load_obj(asset_path("jelly.obj"))
    texture = scene.create_image("texture")
    texture.load(asset_path("jelly.png"))

    # create a base mesh for the animation. The animation
    # will only change the vertex positions, so this mesh
    # is used to set everything else, e.g. textures.
    base_mesh = scene.create_mesh("jelly_base")
    base_mesh.texture_id = texture.image_id
    base_mesh.use_texture_alpha = True
    base_mesh.add_mesh(jelly_mesh)

    def random_linspace(min_val, max_val, num_samples):
        vals = np.linspace(min_val, max_val, num_samples)
        np.random.shuffle(vals)
        return vals

    # this base mesh will be instanced, so we can animate each
    # instance individual using rigid transforms, in this case
    # just translation.
    marbles = scene.create_mesh("marbles_base")
    num_marbles = 10
    marbles.add_sphere(sp.Colors.White, transform=sp.Transforms.Scale(0.2))
    marble_positions = np.zeros((num_marbles, 3), np.float32)
    marble_positions[:, 0] = random_linspace(-0.6, 0.6, num_marbles)
    marble_positions[:, 2] = random_linspace(-1, 0.7, num_marbles)
    marble_offsets = np.random.uniform(0, 2 * np.pi, size=num_marbles).astype(np.float32)
    marble_colors_start = np.random.uniform(0, 1, size=(num_marbles, 3)).astype(np.float32)
    marble_colors_end = np.random.uniform(0, 1, size=(num_marbles, 3)).astype(np.float32)
    marbles.enable_instancing(marble_positions, colors=marble_colors_start)

    for i in range(60):
        # animate the wave mesh by updating the vertex positions
        positions = jelly_mesh.positions.copy()
        delta_x = (positions[:, 0] + 0.0838 * i) * 10
        delta_z = (positions[:, 2] + 0.0419 * i) * 10
        positions[:, 1] = positions[:, 1] + 0.1 * (np.cos(delta_x) + np.sin(delta_z))

        # we create a mesh update with the new posiitons. We can use this mesh update
        # just like a new mesh, because it essentially is one, as ScenePic will create
        # a new mesh from the old one using these new positions.
        jelly_update = scene.update_mesh_positions("jelly_base", positions)
        frame = canvas.create_frame(meshes=[jelly_update])

        # this is a simpler form of animation in which we will change the position
        # and colors of the marbles
        marble_y = np.sin(0.105 * i + marble_offsets)
        positions = np.stack([marble_positions[:, 0], marble_y, marble_positions[:, 2]], -1)
        alpha = ((np.sin(marble_y) + 1) * 0.5).reshape(-1, 1)
        beta = 1 - alpha
        colors = alpha * marble_colors_start + beta * marble_colors_end
        marbles_update = scene.update_instanced_mesh("marbles_base", positions, colors=colors)
        frame.add_mesh(marbles_update)

    scene.quantize_updates()
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
