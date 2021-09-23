"""The Science Visualizaton Toolkit (SVT) python interface module.

All platforms have good support for 2D images, with well-recognized
formats such as PNG and JPEG that can be viewed out of the box (no
installation) and shared trivially.

However, while many formats exist for 3D data, none are well-supported
without installation of tools such as MeshLab, Blender, etc.  Windows
is at the beginning of a journey to fixing that with Windows Mixed
Reality and Paint 3D, but it is still early days.

SVT (Science Visualization Toolkit) was created for 3D computer
vision researchers such as those in Analog (Science).  SVT was
designed as a lightweight, reuseable 3D visualization library,
with the following desiderata in mind:

- Make experimentation with 3D data near effortless
- Incredibly easy to create and share 3D results
- zero-install sharing of detailed 3D results using HTML
- based on modern web standards so usable with any modern
    browser (tested in Edge and Chrome)
- embeddable in other HTML documents
- Performant
- based on WebGL
- High quality visuals
- Works both offline or interactively in client-server setup
- Simple, clean API
- friendly Python front-end
- basic mesh json file format
- other language front ends easy to add
- Extensible to WebVR in future
- use HoloLens in our science workflow
"""

from typing import Tuple, Union

import numpy as np

from . import _svt
from ._svt import (
    Camera,
    ColorFromBytes,
    DropDownMenu,
    FocusPoint,
    Graph,
    js_lib_src,
    Label,
    LayerSettings,
    load_obj,
    LoopSubdivisionStencil,
    MeshInfo,
    PathToSVTLib,
    Shading,
    TextPanel,
    UIParameters,
)
from .audio_track import AudioTrack
from .canvas2d import Canvas2D
from .canvas3d import Canvas3D
from .frame2d import Frame2D
from .frame3d import Frame3D
from .image import Image
from .mesh import Mesh, MeshUpdate, VertexBuffer
from .scene import Scene
from .version import __version__
from .video import Video

try:
    from .video_writer import VideoWriter
except ImportError:
    print("svt[info]> Disabling VideoWriter due to ImportError")

try:
    from .web_socket_handler import WebSocketHandler
except ImportError:
    print("svt[info]> Disabling WebSocketHandler due to ImportError")

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
    "PathToSVTLib",
    "Shading",
    "UIParameters",
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
    "WebSocketHandler",
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

    Black = _svt.Black
    White = _svt.White
    Red = _svt.Red
    Maroon = _svt.Maroon
    Pink = _svt.Pink
    Brown = _svt.Brown
    Orange = _svt.Orange
    Coral = _svt.Coral
    Olive = _svt.Olive
    Yellow = _svt.Yellow
    Beige = _svt.Beige
    Lime = _svt.Lime
    Green = _svt.Green
    Mint = _svt.Mint
    Teal = _svt.Teal
    Cyan = _svt.Cyan
    Navy = _svt.Navy
    Blue = _svt.Blue
    Purple = _svt.Purple
    Lavender = _svt.Lavender
    Magenta = _svt.Magenta
    Gray = _svt.Gray
    Grey = _svt.Gray


class Transforms:
    """Static class with convenience methods for creating various 3D homogeneous transforms."""

    scale = _svt.scale

    @staticmethod
    def Scale(scale: Union[np.ndarray, float]):  # noqa: N802
        """Creates a 3D homogeneous scale matrix.

        Args:
            scale (Union[np.ndarray, float]): the axis-specific scale values

        Returns:
            np.ndarray: a 3D homogeneous transform matrix
        """
        return _svt.scale(scale)

    translate = _svt.translate

    @staticmethod
    def Translate(vec: np.ndarray):  # noqa: N802
        """Creates a 3D homogeneous scale matrix.

        Args:
            vec (np.ndarray): the movement vector

        Returns:
            np.ndarray: a 3D homogeneous transform matrix
        """
        return _svt.translate(vec)

    rotation_matrix_from_axis_angle = _svt.rotation_matrix_from_axis_angle

    @staticmethod
    def RotationMatrixFromAxisAngle(axis: np.ndarray, angle: float):  # noqa: N802
        """Creates a 3D homogeneous scale matrix from axis and angle.

        Args:
            axis (np.ndarray): the axis of rotation
            angle (float): the angle to rotate in radians

        Returns:
            np.ndarray: a 3D homogeneous transform matrix
        """
        return _svt.rotation_matrix_from_axis_angle(axis, angle)

    quaternion_from_axis_angle = _svt.quaternion_from_axis_angle

    @staticmethod
    def QuaternionFromAxisAngle(axis: np.ndarray, angle: float):  # noqa: N802
        """Convert rotation axis and angle to a quaternion.

        Args:
            axis (np.ndarray): the axis of rotation
            angle (float): the angle to rotate in radians

        Returns:
            np.ndarray: a 3D homgenous transform matrix
        """
        return _svt.quaternion_from_axis_angle(axis, angle)

    axis_angle_to_align_x_to_axis = _svt.axis_angle_to_align_x_to_axis

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
        return _svt.axis_angle_to_align_x_to_axis(axis)

    quaternion_to_align_x_to_axis = _svt.quaternion_to_align_x_to_axis

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
        return _svt.quaternion_to_align_x_to_axis(axis)

    rotation_to_align_x_to_axis = _svt.rotation_to_align_x_to_axis

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
        return _svt.rotation_to_align_x_to_axis(axis)

    rotation_about_x = _svt.rotation_about_x

    @staticmethod
    def RotationAboutXAxis(angle: float):  # noqa: N802
        """Creates a 3D homogeneous rotation matrix about the x-axis.

        Args:
            angle (float): the angle to rotation in radians

        Returns:
            np.ndarray: a 3D homogeous transform matrix
        """
        return _svt.rotation_about_x(angle)

    rotation_about_y = _svt.rotation_about_y

    @staticmethod
    def RotationAboutYAxis(angle: float):  # noqa: N802
        """Creates a 3D homogeneous rotation matrix about the y-axis.

        Args:
            angle (float): the angle to rotation in radians

        Returns:
            np.ndarray: a 3D homogeous transform matrix
        """
        return _svt.rotation_about_y(angle)

    rotation_about_z = _svt.rotation_about_z

    @staticmethod
    def RotationAboutZAxis(angle: float):  # noqa: N802
        """Creates a 3D homogeneous rotation matrix about the z-axis.

        Args:
            angle (float): the angle to rotation in radians

        Returns:
            np.ndarray: a 3D homogeous transform matrix
        """
        return _svt.rotation_about_z(angle)

    look_at_rotation = _svt.look_at_rotation
    euler_angles_to_matrix = _svt.euler_angles_to_matrix
    quaternion_to_matrix = _svt.quaternion_to_matrix
    gl_projection = _svt.gl_projection
    gl_world_to_camera = _svt.gl_world_to_camera


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
