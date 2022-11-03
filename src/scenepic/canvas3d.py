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
Canvas3D.create_frame.__doc__ = """Creates a new Frame3D object and appends to the list of Frame3Ds in the Canvas3D.

Args:
    frame_id (str): a unique identifier for the Frame3D (will be
                    automatically populated if not provided).
    focus_point (FocusPoint): optional 3D focus point for this frame (with optional 3D
                            rotation for orientation lock), used in the ScenePic user interface.
    meshes (List[Union[Mesh, MeshUpdate, str]]): optionally pre-populate this Frame3D
                                                with a set of Meshes, for convenience.
    camera (Camera): optionally set a camera for this frame

Returns:
    Frame3D: a new Frame3D object
"""

Canvas3D.set_layer_settings = set_layer_settings
Canvas3D.set_layer_settings.__doc__ = """Specify the visibilities and opacities of certain mesh layers.

Description:
    Each Mesh object can optionally be part of a user-identified layer
    (see Mesh constructor).
    Calling set_layer_settings will result in an additional drop-down
    selector in the ScenePic user interface.

Args:
    self (Canvas3D): the self reference
    layer_settings (Mapping[str, Union[dict, LayerSettings]]): a LayerSettings object
        or a dictionary. See LayerSettings for details.
"""
