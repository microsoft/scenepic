"""Module which extends the scenepic Mesh type."""

# Copyright (c) Microsoft Corporation.
# Licensed under the MIT License.

import numpy as np

from ._scenepic import Mesh, MeshUpdate


class VertexBuffer:
    def __init__(self, values: np.ndarray):
        self._values = values
        self._lookup = {
            "pos": slice(0, 3),
            "norm": slice(3, 6),
            "rgb": slice(6, 9),
            "uv": slice(6, 8)
        }

    @property
    def shape(self) -> tuple:
        return self._values.shape

    def copy(self) -> "VertexBuffer":
        return VertexBuffer(self._values.copy())

    def __repr__(self) -> str:
        return str(self._values)

    def __getitem__(self, key: str) -> np.ndarray:
        assert key in self._lookup, "Invalid vertex buffer key " + key
        return self._values[:, self._lookup[key]]

    def __setitem__(self, key: str, data: np.ndarray):
        assert key in self._lookup, "Invalid vertex buffer key " + key
        self._values[:, self._lookup[key]] = data


def vertex_buffer(self):
    return VertexBuffer(self.get_vertex_buffer())


def quantize(self, keyframe_index: int, fixed_point_range: float, keyframe_vertex_buffer: VertexBuffer):
    self.quantize_(keyframe_index, fixed_point_range, keyframe_vertex_buffer._values)


def difference_range(self, vertex_buffer: VertexBuffer) -> float:
    self.difference_range_(vertex_buffer._values)


Mesh.vertex_buffer = property(vertex_buffer)
MeshUpdate.vertex_buffer = property(vertex_buffer)
MeshUpdate.quantize = quantize
MeshUpdate.difference_range = difference_range
