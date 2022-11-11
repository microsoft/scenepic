"""Module which extends the ScenePic Scene type."""

# Copyright (c) Microsoft Corporation.
# Licensed under the MIT License.

import os
from typing import Union

from ._scenepic import (
    Canvas2D,
    Canvas3D,
    Graph,
    Image,
    js_lib_src,
    Mesh,
    Scene
)


def link_canvas_events(self, *canvases):
    ids = [canvas.canvas_id if isinstance(canvas, (Canvas3D, Canvas2D, Graph)) else canvas
           for canvas in canvases]
    self.link_canvas_events_(ids)


def create_mesh(self, mesh_id="", **kwargs) -> Mesh:
    if "texture_id" in kwargs:
        if isinstance(kwargs["texture_id"], Image):
            kwargs["texture_id"] = kwargs["texture_id"].image_id

    return self.create_mesh_(mesh_id, **kwargs)


def simulate_key_presses(self, canvas_id: Union[Canvas3D, Canvas2D, str], *keypresses):
    if isinstance(canvas_id, (Canvas2D, Canvas3D)):
        canvas_id = canvas_id.canvas_id

    self.simulate_key_presses_(canvas_id, keypresses)


def _repr_javascript_(self):
    if self.script_cleared:
        raise Exception("You should not call clear_script() on Scenes that "
                        "you wish to use in Jupyter.")

    # Jupyter notebooks need to load the library as an external file
    with open("scenepic.min.js", "wb") as file:
        src = js_lib_src()
        file.write(src.encode())

    loader_path = os.path.join(os.path.dirname(__file__),
                               "scene_jupyter_template.js")
    with open(loader_path) as file:
        script = file.read() % self.get_json()

    return script


Scene.link_canvas_events = link_canvas_events
Scene.link_canvas_events.__doc__ = """Specify that all input events should be linked across the provided set of Canvases.

Args:
    self (Scene): self reference
    canvases: the canvases which should have linked events
"""

Scene.create_mesh = create_mesh
Scene.create_mesh.__doc__ = """Create a new Mesh and append to the Scene.

Description:
    Meshes are Scene-wide resources that can be reused across multiple
    Frames in multiple Canvas3Ds.

Args:
    mesh_id (str, optional): a unique identifier for the Mesh (will be
                            automatically populated if not provided). Defaults to None.
    layer_id (str, optional): an identifier to allow several Meshes to be grouped together
                            into a "layer".  Layers should be used in conjunction with
                            Canvas3D.set_layer_settings(). Defaults to None.
    double_sided (bool, optional): whether to turn off back face culling and draw the Mesh's
                                triangles as double sided. Defaults to False.
    camera_space (bool, optional): whether this Mesh is defined in camera space (cannot
                                be moved in the ScenePic user interface) or world space (standard).
                                Defaults to False.
    shared_color (np.ndarray, optional): see `Mesh` constructor.
    texture_id (Union[Image, str], optional): see `Mesh` constructor.
    nn_texture (bool, optional): whether the shader should use nearest-neighbor
                texture interpolation. Defaults to True.
    use_texture_alpha (bool, optional): whether to use the alpha channel in the texture
                                        for transparency (only relevant for textured Meshes).
                                        Defaults to True.
    is_billboard (bool, optional): draw this Mesh as a billboard (i.e. always facing the user)
                                rather than rotating with the rest of the world. Defaults to False.
    is_label (bool, optional): this mesh will be treated specially as a label. Not for public use.
                            Defaults to False.

Returns:
    Mesh: a reference to the new Mesh object
"""


Scene.simulate_key_presses = simulate_key_presses
Scene.simulate_key_presses.__doc__ = """Simulates one or more keypresses to fire off events in the ScenePic UI at startup, e.g. video playback.

Args:
    self (Scene): self reference
    canvas_id (Union[Canvas3D, Canvas2D, str]): ID of the canvas on which to fire off this
                                                event.
    keypresses: the key values
"""

Scene._repr_javascript_ = _repr_javascript_
