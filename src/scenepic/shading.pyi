import numpy as np


class Shading:
    """Parameters of the shaders."""

    def __init__(self, bg_color: np.ndarray, ambient_light_color: np.ndarray,
                 directional_light_color: np.ndarray,
                 directional_light_dir: np.ndarray):
        """Constructor.

        Args:
            bg_color (np.ndarray, optional): background color. Defaults to Black.
            ambient_light_color (np.ndarray, optional): color of ambient light. Defaults to [0.7, 0.7, 0.7].
            directional_light_color (np.ndarray, optional): color of directional light. Defaults to [0.3, 0.3, 0.3].
            directional_light_dir (np.ndarray, optional): direction of directional light. Defaults to [2, 1, 2].
        """

    @property
    def bg_color(self) -> np.ndarray:
        """The background color."""

    @property
    def ambient_light_color(self) -> np.ndarray:
        """The color of ambient light."""

    @property
    def directional_light_color(self) -> np.ndarray:
        """The directional light color."""

    @property
    def directional_light_dir(self) -> np.ndarray:
        """The direction of the directional light."""
