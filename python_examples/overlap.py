"""Test script for dropdown CSS"""

import random
from typing import List
import numpy as np
import scenepic as sp


def _create_cubes(scene: sp.Scene, num_cubes: int) -> List[sp.Mesh]:
    meshes = []
    for i in range(num_cubes):
        color = np.random.rand(3)
        mesh: sp.Mesh = scene.create_mesh(layer_id="cube{}".format(i), shared_color=color)
        mesh.add_cube()
        meshes.append(mesh)

    return meshes


def _add_objects(canvas: sp.Canvas3D, cubes: List[sp.Mesh]):
    frame = canvas.create_frame()
    num_cubes = np.random.randint(5, 10)
    choices = np.random.choice(len(cubes), num_cubes, replace=False)
    for index in choices:
        pos = np.random.rand(3) * 2 - 1
        frame.add_mesh(cubes[index], sp.Transforms.Translate(pos))


def _add_circles(canvas: sp.Canvas2D):
    frame: sp.Frame2D = canvas.create_frame()
    num_circles = np.random.randint(5, 10)
    for i in range(num_circles):
        pos = np.random.rand(2) * 100
        radius = np.random.random() * 10
        color = np.random.rand(3)
        frame.add_circle(pos[0], pos[1], radius, fill_color=color, layer_id="circle{}".format(i))


def _main():
    scene = sp.Scene()

    cubes = _create_cubes(scene, 20)
    for row in range(1, 5):
        for col in range(1, 5):
            canvasID = "canvas" + str(row) + str(col)
            if random.random() < 0.5:
                canvas = scene.create_canvas_3d(canvasID, 100, 100)
                _add_objects(canvas, cubes)
            else:
                canvas = scene.create_canvas_2d(canvasID, 100, 100)
                _add_circles(canvas)

            scene.place(canvasID, str(row), str(col))

    scene.grid("400px", "auto auto auto auto", "auto auto auto auto")
    scene.save_as_html("overlap.html")


if __name__ == "__main__":
    _main()
