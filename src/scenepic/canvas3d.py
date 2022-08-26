"""Module which extends the scenepic Canvas3D type."""

# Copyright (c) Microsoft Corporation.
# Licensed under the MIT License.

from typing import List, Union, Mapping

import numpy as np

from ._scenepic import (
    Camera,
    Canvas3D,
    FocusPoint,
    Frame3D,
    LayerSettings,
    Mesh,
    MeshUpdate
)


def create_frame(self,
                 frame_id="",
                 focus_point: FocusPoint = None,
                 meshes: List[Union[Mesh, MeshUpdate, str]] = None,
                 camera: Camera = None) -> Frame3D:
    kwargs = {}
    if focus_point is not None:
        if isinstance(focus_point, (list, np.ndarray)):
            focus_point = FocusPoint(focus_point)

        kwargs["focus_point"] = focus_point

    if meshes is not None:
        kwargs["mesh_ids"] = [mesh if isinstance(mesh, str) else mesh.mesh_id
                              for mesh in meshes]

    if camera is not None:
        kwargs["camera"] = camera

    return self.create_frame_(frame_id, **kwargs)


def set_layer_settings(self, layer_settings: Mapping[str, Union[dict, LayerSettings]]):
    updated_settings = {}
    for key, value in layer_settings.items():
        if isinstance(value, dict):
            if "visible" in value:
                print("`visible` keyword has been deprecated")
            else:
                updated_settings[key] = LayerSettings(**value)

    self.set_layer_settings_(updated_settings)


Canvas3D.create_frame = create_frame
Canvas3D.set_layer_settings = set_layer_settings
