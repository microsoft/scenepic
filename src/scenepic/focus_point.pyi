import numpy as np


class FocusPoint:
    """3D focus point for this frame (with optional 3D rotation for orientation lock), used in the ScenePic user interface."""

    def __init__(self, positions: np.ndarray, orientation_axis_angle: np.ndarray):
        """Constructor.

        Args:
            positions (np.ndarray): The focus point position
            orientation_axis_angle (np.ndarray, optional): Orientation axis for orientation lock
        """

    @property
    def position(self) -> np.array:
        """The position of the focus point."""

    @property
    def orientation_axis_angle(self) -> np.ndarray:
        """The orientation axis angle"""
