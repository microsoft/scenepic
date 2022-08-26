from typing import List, Mapping, Union

from .focus_point import FocusPoint
from .frame3d import Frame3D
from .camera import Camera
from .layer_settings import LayerSettings
from .mesh import Mesh, MeshUpdate
from .shading import Shading
from .ui_parameters import UIParameters


class Canvas3D:
    """Represents a ScenePic Canvas3D - a 3D viewport that can contain a number of Frame3Ds."""

    def create_frame(self, frame_id: str, focus_point: FocusPoint,
                     meshes: List[Union[Mesh, MeshUpdate, str]],
                     camera: Camera) -> Frame3D:
        """Creates a new Frame3D object and appends to the list of Frame3Ds in the Canvas3D.

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

    def set_layer_settings(self, layer_settings: Mapping[str, Union[dict, LayerSettings]]):
        """Specify the visibilities and opacities of certain mesh layers.

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

    @property
    def canvas_id(self) -> str:
        """A unique identifier for the canvas."""

    def clear_script(self) -> None:
        """Call this if you are dumping a ScenePic script in multiple parts.

        Use this for client-server interactions, or when chunking a large
        visualization into multiple files.
        """

    @property
    def width(self) -> float:
        """The width of the Canvas."""

    @property
    def height(self) -> float:
        """The height of the Canvas."""

    @property
    def camera(self) -> Camera:
        """Set of Camera parameters for the Canvas."""

    @property
    def shading(self) -> Shading:
        """Set of Shading parameters for the Canvas."""

    @property
    def focus_point(self) -> FocusPoint:
        """Default focus point for the canvas."""

    @property
    def ui_parameters(self) -> UIParameters:
        """Set of user interface parameters."""

    @property
    def media_id(self) -> str:
        """The ID of the attached media file.

        This file will be used to drive playback, i.e. frames will be
        displayed in time with the playback of the media file.
        """
