from typing import List, Mapping, Union

import numpy as np

from .camera import Camera
from .focus_point import FocusPoint
from .label import Label
from .mesh import Mesh, MeshUpdate
from .layer_settings import LayerSettings


class Frame3D:
    """Represents a frame of an animation which contains a number of transformed Mesh objects."""

    def add_meshes(self, meshes: List[Union[Mesh, MeshUpdate, str]], transform: np.ndarray):
        """Adds meshes to the frame.

        Args:
            meshes (List[Union[Mesh, MeshUpdate, str]]): A list of meshes and mesh updates (or their IDs)
            transform (np.ndarray, optional): an optional transform to apply to each mesh. Defaults to None.
        """

    def add_mesh(self, mesh: Union[Mesh, MeshUpdate, str], transform: np.ndarray):
        """Adds a mesh to the frame.

        Args:
            mesh (Union[Mesh, MeshUpdate, str]): The mesh, mesh update, or mesh ID
            transform (np.ndarray, optional): An optional transform to apply to the mesh. Defaults to None.
        """

    def add_label(self, label: Label, position: np.ndarray) -> None:
        """Add a label to the frame.

        Args:
            label (Label): the label to add
            position (np.ndarray): the float32 [x, y, z] position at which to put the label
        """

    def set_layer_settings(self, layer_settings: Mapping[str, Union[dict, LayerSettings]]):
        """Specify the visibilities and opacities of certain mesh layers.

        Description:
            Each Mesh object can optionally be part of a user-identified layer
            (see Mesh constructor).
            Calling set_layer_settings will result in an additional drop-down
            selector in the ScenePic user interface.

        Args:
            layer_settings (Mapping[str, Union[dict, LayerSettings]]): a LayerSettings object
                or a dictionary. See LayerSettings for details.
        """

    @property
    def focus_point(self) -> FocusPoint:
        """The focus point of the frame"""

    @property
    def camera(self) -> Camera:
        """The camera for the frame.

        This property can be used to create cinematic camera movement within a
        ScenePic, but the user can choose to override the camera
        track at any time. If they reset to the original camera, however, it will make
        subsequent frames use the specified camera parameters.
        """
