import numpy as np

from .mesh_info import MeshInfo


def js_lib_src() -> str:
    """Returns the Javascript ScenePic library as a string."""


def ColorFromBytes(red: int, green: int, blue: int) -> np.ndarray:
    """Construct a color object from byte values.

    Args:
        red (int): Red channel
        green (int): Green channel
        blue (int): Blue channel

    Returns:
        np.ndarray: a valid color object
    """


def load_obj(path: str) -> MeshInfo:
    """Loads a WaveFront OBJ file from disk as a MeshInfo object.

    Args:
        path (str): the path to the OBJ file on disk

    Returns:
        MeshInfo: contains the positions, a triangulation, and UVs (if present)
    """
