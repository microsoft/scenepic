from typing import Union

import numpy as np

from .image import Image


class Frame2D:
    """Represents a frame of a 2D animation."""

    def add_line(self, coordinates: np.ndarray, line_color: np.ndarray,
                 line_width: float, fill_color: np.ndarray, close_path: bool,
                 layer_id: str) -> None:
        """Add a line to the frame.

        Args:
            coordinates (np.ndarray): float32 matrix of [N, 2] coordinates of the line
            line_color (np.ndarray, optional): the color of the line. Defaults to Black.
            line_width (float, optional): the width of the line. Defaults to 1.0.
            fill_color (np.ndarray, optional): the fill color to use. Defaults to None.
            close_path (bool, optional): whether to close the path. Defaults to False.
            layer_id (str, optional): Unique ID of the layer associated with this primitive. Defaults to "".
        """

    def add_rectangle(self, x: float, y: float, w: float, h: float,
                      line_color: np.ndarray, line_width: float,
                      fill_color: np.ndarray, layer_id: str) -> None:
        """Add a rectangle to the frame.

        Args:
            x (float): the upper left corner x coordinate
            y (float): the upper left corner y coordinate
            w (float): the width of the rectangle
            h (float): the height of the rectangle
            line_color (np.ndarray, optional): the color of the line. Defaults to Black.
            line_width (float, optional): the width of the line. Defaults to 1.0.
            fill_color (np.ndarray, optional): the fill color to use. Defaults to None.
            layer_id (str, optional): Unique ID of the layer associated with this primitive. Defaults to "".
        """

    def add_video(self, position_type: str, x: float, y: float, scale: float,
                  smoothed: bool, layer_id: str) -> None:
        """Add an frame from a video to the frame.

        Args:
            position_type (str, optional): one of "fit", "fill", "stretch", or "manual". Defaults to "fit".
            x (float, optional): x coordinate to draw video at if position_type == "manual". Defaults to 0.0.
            y (float, optional): y coordinate to draw video at if position_type == "manual". Defaults to 0.0.
            scale (float, optional): scale factor to draw video by if position_type == "manual". Defaults to 1.0.
            smoothed (bool, optional): whether to draw an video with smoothing or using nearest neighbor. Defaults to False.
            layer_id (str, optional): Unique ID of the layer associated with this primitive. Defaults to "".
        """

    def add_image(self, image_id: Union[Image, str], position_type: str,
                  x: float, y: float, scale: float, smoothed: bool,
                  layer_id: str) -> None:
        """Add an image to the frame.

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

    def add_circle(self, x: float, y: float, radius: float,
                   line_color: np.ndarray, line_width: float,
                   fill_color: np.ndarray, layer_id: str) -> None:
        """Add a circle to the frame.

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

    def add_text(self, text: str, left: float, bottom: float,
                 color: np.ndarray, size_in_pixels: float, font_family: str,
                 layer_id: str):
        """Add some text to the frame.

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
