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
Frame2D.add_circle = add_circle
Frame2D.add_text = add_text
