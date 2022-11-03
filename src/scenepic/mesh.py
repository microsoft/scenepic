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


Mesh.vertex_buffer = property(vertex_buffer, doc="""The raw vertex buffer.""")

MeshUpdate.vertex_buffer = property(vertex_buffer, doc="""The raw vertex buffer.""")

MeshUpdate.quantize = quantize
MeshUpdate.quantize.__doc__ = """Quantize the mesh update.

Args:
    self (MeshUpdate): self reference
    keyframe_index (int): Index of the keyframe to use in quantizing this update
    fixed_point_range (float): The range to use for the fixed point representation.
    keyframe_vertex_buffer (VertexBuffer): The keyframe vertex buffer
"""

MeshUpdate.difference_range = difference_range
MeshUpdate.difference_range.__doc__ = """Returns the absolute range of values in the difference between this update and the buffer.

Args:
    self (MeshUpdate): self reference
    vertex_buffer (VertexBuffer): the buffer to use in the comparison

Return:
    float: the absolute range (from minimum to maximum) in the per-index difference between
        this update and the reference.
"""
