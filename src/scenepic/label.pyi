import numpy as np


class Label:
    """A ScenePic label type"""

    @property
    def label_id(self) -> str:
        """A unique identifier for the Label"""

    @property
    def mesh_id(self) -> str:
        """The mesh associated with this label."""

    @property
    def text(self) -> str:
        """The text to use in the label."""

    @property
    def fill_color(self) -> np.ndarray:
        """The color to use."""

    @property
    def size_in_pixels(self) -> float:
        """The size in pixels at which to display the label."""

    @property
    def font_family(self) -> str:
        """HTML font family name."""

    @property
    def horizontal_align(self) -> str:
        """How to align the label horizontally (one of 'left', 'center', 'right')"""

    @property
    def offset_distance(self) -> float:
        """Distance in world space to offset the label by when using left/right/top/bottom alignment."""
    