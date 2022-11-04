"""Module which extends the scenepic Frame3D type."""

# Copyright (c) Microsoft Corporation.
# Licensed under the MIT License.

from typing import List, Mapping, Union

import numpy as np

from ._scenepic import Frame3D, LayerSettings, Mesh, MeshUpdate


def add_meshes(self, meshes: List[Union[Mesh, MeshUpdate, str]], transform: np.ndarray = None):
    mesh_ids = [mesh if isinstance(mesh, str) else mesh.mesh_id
                for mesh in meshes]

    if transform is None:
        self.add_meshes_(mesh_ids)
    else:
        self.add_meshes_(mesh_ids, transform)


def add_mesh(self, mesh: Union[Mesh, MeshUpdate, str], transform: np.ndarray = None):
    mesh_id = mesh if isinstance(mesh, str) else mesh.mesh_id

    if transform is None:
        self.add_mesh_(mesh_id)
    else:
        self.add_mesh_(mesh_id, transform)


def set_layer_settings(self, layer_settings: Mapping[str, Union[dict, LayerSettings]]):
    updated_settings = {}
    for key, value in layer_settings.items():
        if isinstance(value, dict):
            if "visible" in value:
                print("`visible` keyword has been deprecated")
            else:
                updated_settings[key] = LayerSettings(**value)

    self.set_layer_settings_(updated_settings)


Frame3D.add_meshes = add_meshes
Frame3D.add_meshes.__doc__ = """Adds meshes to the frame.

Args:
    meshes (List[Union[Mesh, MeshUpdate, str]]): A list of meshes and mesh updates (or their IDs)
    transform (np.ndarray, optional): an optional transform to apply to each mesh. Defaults to None.
"""

Frame3D.add_mesh = add_mesh
Frame3D.add_mesh.__doc__ = """Adds a mesh to the frame.

Args:
    mesh (Union[Mesh, MeshUpdate, str]): The mesh, mesh update, or mesh ID
    transform (np.ndarray, optional): An optional transform to apply to the mesh. Defaults to None.
"""

Frame3D.set_layer_settings = set_layer_settings
Frame3D.set_layer_settings.__doc__ = """Specify the visibilities and opacities of certain mesh layers.

Description:
    Each Mesh object can optionally be part of a user-identified layer
    (see Mesh constructor).
    Calling set_layer_settings will result in an additional drop-down
    selector in the ScenePic user interface.

Args:
    layer_settings (Mapping[str, Union[dict, LayerSettings]]): a LayerSettings object
        or a dictionary. See LayerSettings for details.
"""
