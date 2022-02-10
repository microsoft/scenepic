"""Module which extends the Camera type."""

# Copyright (c) Microsoft Corporation.
# Licensed under the MIT License.

from ._scenepic import (
    Camera,
    camera_orbit
)

Camera.orbit = camera_orbit
