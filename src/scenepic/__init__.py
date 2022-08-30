"""ScenePic python interface module.

All platforms have good support for 2D images, with well-recognized formats
such as PNG and JPEG that can be viewed out of the box (no installation)
and shared trivially.

However, while many formats exist for 3D data, none are well-supported
without installation of tools such as MeshLab, Blender, etc.

ScenePic was created for 3D computer vision researchers such as those
working on [HoloLens](https://www.microsoft.com/en-gb/hololens)
and [Mesh](https://www.microsoft.com/en-us/mesh) at Microsoft.
It was designed to be a lightweight, reuseable 3D visualization
library, with the following desiderata in mind:

- Make experimentation with 3D data near effortless
- Incredibly easy to create and share 3D results
  * zero-install sharing of detailed 3D results using HTML
  * based on modern web standards so usable with any modern browser
    (tested in Edge, FireFox and Chrome)
  * embeddable in other HTML documents
- Performant
  * based on WebGL
- High quality visuals
- Works both offline or interactively in client-server setup
- Simple, clean API
  * friendly Python front-end
  * basic mesh json file format
  * other language front ends easy to add
"""

from typing import Tuple, Union

import numpy as np

from . import _scenepic
from .audio_track import AudioTrack
from .camera import Camera
from .canvas2d import Canvas2D
from .canvas3d import Canvas3D
from .drop_down_menu import DropDownMenu
from .focus_point import FocusPoint
from .frame2d import Frame2D
from .frame3d import Frame3D
from .globals import ColorFromBytes, js_lib_src, load_obj
from .graph import Graph
from .image import Image
from .label import Label
from .layer_settings import LayerSettings
from .loop_subdivision_stencil import LoopSubdivisionStencil
from .mesh import Mesh, MeshUpdate, VertexBuffer
from .mesh_info import MeshInfo
from .scene import Scene
from .shading import Shading
from .text_panel import TextPanel
from .ui_parameters import UIParameters
from .version import __version__
from .video import Video

try:
    from .video_writer import VideoWriter
except ImportError:
    print("scenepic[info]> Disabling VideoWriter due to ImportError")

__all__ = [
    "__version__",
    "AudioTrack",
    "Camera",
    "Canvas2D",
    "ColorFromBytes",
    "DropDownMenu",
    "FocusPoint",
    "Frame3D",
    "Graph",
    "js_lib_src",
    "MeshInfo",
    "Label",
    "LayerSettings",
    "load_obj",
    "Shading",
    "UIParameters",
    "QuantizationOptions",
    "Canvas3D",
    "Frame2D",
    "Image",
    "LoopSubdivisionStencil",
    "Mesh",
    "MeshUpdate",
    "Scene",
    "TextPanel",
    "VertexBuffer",
    "Video",
    "VideoWriter",
]


def Color(r: float, g: float, b: float) -> np.ndarray:  # noqa: N802
    """Convenience method for creating a numpy 3-vector from R,G,B float values.

    Args:
        r (float): red value [0,1].
        g (float): green value [0,1].
        b (float): blue value [0,1].

    Raises:
        ValueError: Will raise this if the values are out of range

    Returns:
        np.ndarray: a color array
    """
    if 0.0 <= r <= 1.0 and 0.0 <= g <= 1.0 and 0.0 <= b <= 1.0:
        return np.array([r, g, b], dtype=np.float32)

    raise ValueError("Color values range: [0,1]")


class Colors:
    """Class containing standard colors.

    Attributes:
        Black (np.ndarray): (0, 0, 0)
        White (np.ndarray): (255, 255, 255)
        Red (np.ndarray): (255, 0, 0)
        Maroon (np.ndarray): (128, 0, 0)
        Pink (np.ndarray): (255, 200, 220)
        Brown (np.ndarray): (170, 110, 40)
        Orange (np.ndarray): (255, 150, 0)
        Coral (np.ndarray): (255, 215, 180)
        Olive (np.ndarray): (128, 128, 0)
        Yellow (np.ndarray): (255, 235, 0)
        Beige (np.ndarray): (255, 250, 200)
        Lime (np.ndarray): (190, 255, 0)
        Green (np.ndarray): (0, 190, 0)
        Mint (np.ndarray): (170, 255, 195)
        Teal (np.ndarray): (0, 128, 128)
        Cyan (np.ndarray): (100, 255, 255)
        Navy (np.ndarray): (0, 0, 128)
        Blue (np.ndarray): (67, 133, 255)
        Purple (np.ndarray): (130, 0, 150)
        Lavender (np.ndarray): (230, 190, 255)
        Magenta (np.ndarray): (255, 0, 255)
        Gray (np.ndarray): (128, 128, 128)
    """

    Black = _scenepic.Black
    White = _scenepic.White
    Red = _scenepic.Red
    Maroon = _scenepic.Maroon
    Pink = _scenepic.Pink
    Brown = _scenepic.Brown
    Orange = _scenepic.Orange
    Coral = _scenepic.Coral
    Olive = _scenepic.Olive
    Yellow = _scenepic.Yellow
    Beige = _scenepic.Beige
    Lime = _scenepic.Lime
    Green = _scenepic.Green
    Mint = _scenepic.Mint
    Teal = _scenepic.Teal
    Cyan = _scenepic.Cyan
    Navy = _scenepic.Navy
    Blue = _scenepic.Blue
    Purple = _scenepic.Purple
    Lavender = _scenepic.Lavender
    Magenta = _scenepic.Magenta
    Gray = _scenepic.Gray
    Grey = _scenepic.Gray


class Transforms:
    """Static class with convenience methods for creating various 3D homogeneous transforms."""

    scale = _scenepic.scale

    @staticmethod
    def Scale(scale: Union[np.ndarray, float]):  # noqa: N802
        """Creates a 3D homogeneous scale matrix.

        Args:
            scale (Union[np.ndarray, float]): the axis-specific scale values

        Returns:
            np.ndarray: a 3D homogeneous transform matrix
        """
        return _scenepic.scale(scale)

    translate = _scenepic.translate

    @staticmethod
    def Translate(vec: np.ndarray):  # noqa: N802
        """Creates a 3D homogeneous scale matrix.

        Args:
            vec (np.ndarray): the movement vector

        Returns:
            np.ndarray: a 3D homogeneous transform matrix
        """
        return _scenepic.translate(vec)

    rotation_matrix_from_axis_angle = _scenepic.rotation_matrix_from_axis_angle

    @staticmethod
    def RotationMatrixFromAxisAngle(axis: np.ndarray, angle: float):  # noqa: N802
        """Creates a 3D homogeneous scale matrix from axis and angle.

        Args:
            axis (np.ndarray): the axis of rotation
            angle (float): the angle to rotate in radians

        Returns:
            np.ndarray: a 3D homogeneous transform matrix
        """
        return _scenepic.rotation_matrix_from_axis_angle(axis, angle)

    quaternion_from_axis_angle = _scenepic.quaternion_from_axis_angle

    @staticmethod
    def QuaternionFromAxisAngle(axis: np.ndarray, angle: float):  # noqa: N802
        """Convert rotation axis and angle to a quaternion.

        Args:
            axis (np.ndarray): the axis of rotation
            angle (float): the angle to rotate in radians

        Returns:
            np.ndarray: a 3D homgenous transform matrix
        """
        return _scenepic.quaternion_from_axis_angle(axis, angle)

    axis_angle_to_align_x_to_axis = _scenepic.axis_angle_to_align_x_to_axis

    @staticmethod
    def AxisAngleRotationToRotateXAxisToAlignWithAxis(axis: np.ndarray):  # noqa: N802
        """Find the axis and angle of rotation that will ensure the x-axis aligns with the provided axis.

        Note:
            One degree of freedom is underconstrained.

        Args:
            axis (np.ndarray): the axis to which to align the x-axis

        Returns:
            np.ndarray: a 3D homogeneous transform matrix
        """
        return _scenepic.axis_angle_to_align_x_to_axis(axis)

    quaternion_to_align_x_to_axis = _scenepic.quaternion_to_align_x_to_axis

    @staticmethod
    def QuaternionToRotateXAxisToAlignWithAxis(axis: np.ndarray):  # noqa: N802
        """Creates a quaternion rotation vector that will ensure the x-axis aligns with the provided axis.

        Note:
            One degree of freedom is underconstrained.

        Args:
            axis (np.ndarray): the axis to which to align the x-axis

        Returns:
            np.ndarray: a quaternion
        """
        return _scenepic.quaternion_to_align_x_to_axis(axis)

    rotation_to_align_x_to_axis = _scenepic.rotation_to_align_x_to_axis

    @staticmethod
    def RotationToRotateXAxisToAlignWithAxis(axis: np.ndarray):  # noqa: N802
        """Creates a 3D homogeneous rotation matrix that will ensure the x-axis aligns with the provided axis.

        Note:
            One degree of freedom is underconstrained.

        Args:
            axis (np.ndarray): the axis to which to align the x-axis

        Returns:
            np.ndarray: a 3D homogeneous transform matrix
        """
        return _scenepic.rotation_to_align_x_to_axis(axis)

    rotation_about_x = _scenepic.rotation_about_x

    @staticmethod
    def RotationAboutXAxis(angle: float):  # noqa: N802
        """Creates a 3D homogeneous rotation matrix about the x-axis.

        Args:
            angle (float): the angle to rotation in radians

        Returns:
            np.ndarray: a 3D homogeous transform matrix
        """
        return _scenepic.rotation_about_x(angle)

    rotation_about_y = _scenepic.rotation_about_y

    @staticmethod
    def RotationAboutYAxis(angle: float):  # noqa: N802
        """Creates a 3D homogeneous rotation matrix about the y-axis.

        Args:
            angle (float): the angle to rotation in radians

        Returns:
            np.ndarray: a 3D homogeous transform matrix
        """
        return _scenepic.rotation_about_y(angle)

    rotation_about_z = _scenepic.rotation_about_z

    @staticmethod
    def RotationAboutZAxis(angle: float):  # noqa: N802
        """Creates a 3D homogeneous rotation matrix about the z-axis.

        Args:
            angle (float): the angle to rotation in radians

        Returns:
            np.ndarray: a 3D homogeous transform matrix
        """
        return _scenepic.rotation_about_z(angle)

    look_at_rotation = _scenepic.look_at_rotation
    euler_angles_to_matrix = _scenepic.euler_angles_to_matrix
    quaternion_to_matrix = _scenepic.quaternion_to_matrix
    gl_projection = _scenepic.gl_projection
    gl_world_to_camera = _scenepic.gl_world_to_camera
    quaternion_multiply = _scenepic.quaternion_multiply


class LoopSubdivStencil:
    """Backwards compatibility wrapper for LoopSubdivisionStencil.

    Args:
        triangles (np.ndarray): an Nx3 numpy integer array specifying vertex
                                indices for each triangle.
        steps (int, optional): specifies how many steps of subdivision to apply.
                               Defaults to 1.
        project_to_limit (bool, optional): specifies whether the vertices should be
                                           projected onto the limit surface in the
                                           final step of subdivision. Defaults to False.
    """

    def __init__(self, triangles: np.ndarray, steps=1, project_to_limit=False):
        self._stencil = LoopSubdivisionStencil.create(triangles, steps, project_to_limit)

    def apply(self, vertices: np.ndarray) -> Tuple[np.ndarray, np.ndarray]:
        """Apply a pre-computed subdivision stencil to a set of vertices.

        The same stencil can be reused across many sets of vertices,
        assuming the triangle topology does not change.

        Args:
            vertices (np.ndarray): float32 matrix of [N, 3] vertex positions

        Returns:
            Tuple[np.ndarray, np.ndarray]: new vertices, new triangles
        """
        return self._stencil.apply(vertices), self._stencil.triangles
