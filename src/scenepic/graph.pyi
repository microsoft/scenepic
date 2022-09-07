from typing import List

import numpy as np


class Graph:
    """A 2D viewport that animates one or more sparklines."""

    class Margin:
        """Represents the margin along the edges of a graph."""

        def __init__(self, size: float, top: float, right: float,
                     bottom: float, left: float):
            """Constructor.

            Args:
                size (float, optional): space in pixels to use for all margins. Default 10.
                top (float, optional): the top margin value in pixels
                right (float, optional): the right margin value in pixels
                bottom (float, optional): the bottom margin value in pixels
                left (float, optional): the left margin value in pixels
            """

        top: float
        """The top margin in pixels."""
        right: float
        """The top margin in pixels."""
        bottom: float
        """The top margin in pixels."""
        left: float
        """The top margin in pixels."""

    class VerticalRule:
        """Represents a vertical line in a sparkline graph."""

        def __init__(self, frame: int, color: np.ndarray, line_width: float):
            """Constructor.

            Args:
                frame (int): Frame to add the vertical rule
                color: (Color, optional): the line color. Defauts to black.
                line_width: (float, optional): The line width in pixels. Defaults to 1.0f.
            """

        frame: int
        """The frame at which to add the line."""
        color: np.ndarray
        """The color of the line."""
        line_width: float
        """The width of the line in pixels."""

    @property
    def canvas_id(self) -> str:
        """A unique identifier for the canvas."""

    @property
    def background_color(self) -> np.ndarray:
        """The background color of the frame."""

    @property
    def margin(self) -> Margin:
        """The outside margin of the graph."""

    @property
    def font_family(self) -> str:
        """The font family used for the graph labels."""

    @property
    def name_size(self) -> float:
        """The size of the sparkline labels in pixels."""

    @property
    def value_size(self) -> float:
        """The size of the sparkline values in pixels."""

    @property
    def name_align(self) -> str:
        """The alignment of the sparkline labels.
        One of ('left', 'right', 'top', 'bottom', or 'none')
        """

    @property
    def value_align(self) -> str:
        """The alignment of the sparkline values.        
        One of ('left', 'right', 'top', 'bottom', or 'none')
        """


    @property
    def media_id(self) -> str:
        """The ID of the attached media file.

        This file will be used to drive playback, i.e. frames will be
        displayed in time with the playback of the media file.
        """

    def add_sparkline(self, name: str, values: np.ndarray,
                      line_color: np.ndarray, line_width: float,
                      vertical_rules: List[VerticalRule] = None) -> None:
        """Adds a sparkline to the graph.

        Args:
            name (str): the name of the measured quantity
            values (np.ndarray): the measured values
            line_color (Color, optional): the color of the line (and its labels). Defaults to Black.
            line_width (float, optional): The width of the line. Defaults to 1.0.
            vertical_rules (List[VerticalRule], optional): the vertical rules to add. Defaults to None.
        """
