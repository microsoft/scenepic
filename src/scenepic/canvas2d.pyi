from typing import Mapping, Union

import numpy as np

from .frame2d import Frame2D
from .layer_settings import LayerSettings


class Canvas2D:
    """A 2D canvas that can contain a number of Frame2Ds."""

    @property
    def canvas_id(self) -> str:
        """A unique identifier for the canvas."""

    def create_frame(self, frame_id: str) -> Frame2D:
        """Creates a new Frame2D object and appends to the list of Frame2Ds in the Canvas2D.

        Args:
            frame_id (str, optional): a unique identifier for the Frame2D (will be
                                      automatically populated if not provided).

        Returns:
            Frame2D: a new Frame2D object
        """

    def set_layer_settings(self, layer_settings: Mapping[str, Union[dict, LayerSettings]]):
        """Specify the visibilities and opacities of certain mesh layers.

        Description:
            Each Mesh object can optionally be part of a user-identified layer
            (see Mesh constructor).
            Calling set_layer_settings will result in an additional drop-down
            selector in the ScenePic user interface.

        Args:
            self (Canvas2D): the self reference
            layer_settings (Mapping[str, Union[dict, LayerSettings]]): a mapping from
                layer IDs to layer settings.
        """

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
    def background_color(self) -> np.ndarray:
        """The background color of the frame."""

    @property
    def media_id(self) -> np.ndarray:
        """The ID of the attached media file.

        This file will be used to drive playback, i.e. frames will be
        displayed in time with the playback of the media file.
        """
