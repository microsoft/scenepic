"""Module which extends the ScenePic Scene type."""

# Copyright (c) Microsoft Corporation.
# Licensed under the MIT License.

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

    script = """
{};
window.Element = element;
{}
""".format(self.get_script(), js_lib_src())
    return script


Scene.link_canvas_events = link_canvas_events
Scene.create_mesh = create_mesh
Scene.simulate_key_presses = simulate_key_presses
Scene._repr_javascript_ = _repr_javascript_
