"""Module which extends the scenepic Frame3D type."""

# Copyright (c) Microsoft Corporation.
# Licensed under the MIT License.

from typing import List, Union

import numpy as np

from ._scenepic import Frame3D, Mesh, MeshUpdate


def add_meshes(self, meshes: List[Union[Mesh, MeshUpdate, str]], transform: np.ndarray = None):
    """Adds meshes to the frame.

    Args:
        self (Frame3D): self reference
        meshes (List[Union[Mesh, MeshUpdate, str]]): A list of meshes and mesh updates (or their IDs)
        transform (np.ndarray, optional): an optional transform to apply to each mesh. Defaults to None.
    """
    mesh_ids = [mesh if isinstance(mesh, str) else mesh.mesh_id
                for mesh in meshes]

    if transform is None:
        self.add_meshes_(mesh_ids)
    else:
        self.add_meshes_(mesh_ids, transform)


def add_mesh(self, mesh: Union[Mesh, MeshUpdate, str], transform: np.ndarray = None):
    """Adds a mesh to the frame.

    Args:
        self (Frame3D): self reference
        mesh (Union[Mesh, MeshUpdate, str]): The mesh, mesh update, or mesh ID
        transform (np.ndarray, optional): An optional transform to apply to the mesh. Defaults to None.
    """
    mesh_id = mesh if isinstance(mesh, str) else mesh.mesh_id

    if transform is None:
        self.add_mesh_(mesh_id)
    else:
        self.add_mesh_(mesh_id, transform)


Frame3D.add_meshes = add_meshes
Frame3D.add_mesh = add_mesh
