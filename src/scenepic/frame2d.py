"""Module which extends the scenepic Frame2D type."""

# Copyright (c) Microsoft Corporation.
# Licensed under the MIT License.

from typing import Union

import numpy as np

from ._scenepic import Frame2D, Image


def add_image(self, image_id: Union[Image, str], position_type="fit", x=0.0,
              y=0.0, scale=1.0, smoothed=False, layer_id=""):
    if isinstance(image_id, Image):
        image_id = image_id.image_id

    self.add_image_(image_id, position_type, x, y, scale, smoothed, layer_id)


def add_circle(self, x: float, y: float, radius: float,
               line_color=None, line_width=1.0, fill_color=None,
               layer_id=""):
    kwargs = {
        "line_width": line_width,
        "layer_id": layer_id
    }
    if line_color is not None:
        kwargs["line_color"] = np.array(line_color, np.float32)[:3]

    if fill_color is not None:
        kwargs["fill_color"] = np.array(fill_color, np.float32)[:3]

    self.add_circle_(x, y, radius, **kwargs)


def add_text(self, text: str, left: float, bottom: float,
             color=None, size_in_pixels=12.0, font_family="sans-serif",
             layer_id=""):
    kwargs = {
        "size_in_pixels": size_in_pixels,
        "font_family": font_family,
        "layer_id": layer_id
    }

    if color is not None:
        kwargs["color"] = np.array(color, np.float32)[:3]

    self.add_text_(text, left, bottom, **kwargs)


Frame2D.add_image = add_image
Frame2D.add_image.__doc__ = """Add an image to the frame.

Args:
    self (Frame2D): the self reference
    image_id (str): the unique identifier of the image
    position_type (str, optional): one of "fit", "fill", "stretch", or "manual". Defaults to "fit".
    x (float, optional): x coordinate to draw image at if position_type == "manual". Defaults to 0.0.
    y (float, optional): y coordinate to draw image at if position_type == "manual". Defaults to 0.0.
    scale (float, optional): scale factor to draw image by if position_type == "manual". Defaults to 1.0.
    smoothed (bool, optional): whether to draw an image with smoothing or using nearest neighbor. Defaults to False.
    layer_id (str, optional): Unique ID of the layer associated with this primitive. Defaults to "".
"""

Frame2D.add_circle = add_circle
Frame2D.add_circle.__doc__ = """Add a circle to the frame.

Args:
    self (Frame2D): the self reference
    x (float): the center x coordinate
    y (float): the center y coordinate
    radius (float): the radius of the circle
    line_color (np.ndarray, optional): the color of the line. Defaults to Black.
    line_width (float, optional): the width of the line. Defaults to 1.0.
    fill_color (np.ndarray, optional): the fill color to use. Defaults to None.
    layer_id (str, optional): Unique ID of the layer associated with this primitive. Defaults to "".
"""

Frame2D.add_text = add_text
Frame2D.add_text.__doc__ = """Add some text to the frame.

Args:
    self (Frame2D): The self reference
    text (str): the text to display
    left (float): the pixel position of the left side of the text
    bottom (float): the pixel position of the bottom of the text
    color (np.ndarray, optional): the color of the text. Defaults to White.
    size_in_pixels (float, optional): the vertical size of the text in pixels. Defaults to 12.0.
    font_family (str, optional): the font to use for the text. Defaults to "sans-serif".
    layer_id (str, optional): Unique ID of the layer associated with this primitive. Defaults to "".
"""
