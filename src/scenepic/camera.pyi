from typing import List

import numpy as np


class Camera:
    """Represents a camera in the scene."""

    def __init__(self, center: np.ndarray, look_at: np.ndarray, up_dir: np.ndarray,
                 fov_y_degrees: float, near_crop_distance: float,
                 far_crop_distance: float, rotation: np.ndarray,
                 world_to_camera: np.ndarray, projection: np.ndarray,
                 aspect_ratio: float):
        """Constructor.

        Note:
            The camera can be created either with the look at paradigm (center,
            look_at, up_dir), with center and rotation, or with an explicit
            world-to-camera matrix.

        Args:
            center (np.ndarray, optional): the camera center. Defaults to [0, 0, 4].
            look_at (np.ndarray, optional): where the camera is looking. Defaults to [0, 0, 0].
            up_dir (np.ndarray, optional): the direction that is "up" for the camera. Defaults to [0, 1, 0].
            fov_y_degrees (float, optional): vertical field of view in degrees. Defaults to 45.0.
            near_crop_distance (float, optional): the near clipping plane of the view frustum. Defaults to 0.01.
            far_crop_distance (float, optional): the far clipping plane of the view frustum. Defaults to 20.0.
            rotation (np.ndarray, optional): a homogeneous rotation transform.
            world_to_camera (np.ndarray, optional): a homogeneous 3D transform mapping world coordinates to camera space.
            projection (np.ndarray, optional): the WebGL projection matrix from camera view coordinates to NDC.
            aspect_ratio (float, optional): The aspect ratio of the camera. Defaults to 1.0.
        """

    @property
    def aspect_ratio(self) -> float:
        """The aspect ratio of the camera."""

    @property
    def center(self) -> np.ndarray:
        """The camera center."""

    @property
    def look_at(self) -> np.ndarray:
        """Where the camera is looking."""

    @property
    def up_dir(self) -> np.ndarray:
        """The direction that is \"up\" for the camera."""

    @property
    def rotation(self) -> np.ndarray:
        """The rotation matrix."""

    @property
    def world_to_camera(self) -> np.ndarray:
        """Homogeneous 3D transform mapping world coordinates to camera space."""

    @property
    def camera_to_world(self) -> np.ndarray:
        """Homogeneous 3D transform that places the camera in the scene."""

    @property
    def projection(self) -> np.ndarray:
        """WebGL projection matrix from camera view coordinates to NDC."""

    @staticmethod
    def camera_orbit(num_frames: int, distance: float, num_times: int,
                     min_altitutde: float, max_altitude: float,
                     up_dir: np.ndarray, fowrard_dir: np.ndarray,
                     fov_y_degrees: float, aspect_ratio: float,
                     near_crop_distance: float,
                     far_crop_distance: float) -> List["Camera"]:
        """Creates a sequence of cameras which move in an orbit around the origin.

        Args:
            num_frames (int): the number of frames in the sequence
            distance (float): the distance of the camera from the origin
            num_times (int): the number of times to orbit
            min_altitude (float): the minimum altitude of the orbit
            max_altitude (float): the maximum altitude of the orbit
            up_dir (np.ndarray): the direct that is up (as a normalized vector)
            forward_dir (np.ndarray): the direction that is forward (as a normalized vector)
            fov_y_degrees (float): the field of the view of the camera in degrees
            aspect_ratio (float): the aspect ratio of the camera
            near_crop_distance (float): the near clipping plane of the view frustum
            far_crop_distance (float): the far clipping plane of the view frustum

        Returns:
            List[Camera]: a sequence of cameras moving in an orbit
        """
