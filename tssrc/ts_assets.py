"""Script to create the holodeck assets."""

import scenepic as sp


def _focuspoint_mesh():
    scene = sp.Scene()
    mesh = scene.create_mesh(shared_color=sp.Color(1.0, 0.0, 1.0))
    mesh.add_cube()
    with open("focus_point.json", "w") as file:
        file.write(str(mesh))


if __name__ == "__main__":
    _focuspoint_mesh()
