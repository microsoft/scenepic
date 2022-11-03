"""Module which extends the scenepic Video type."""

# Copyright (c) Microsoft Corporation.
# Licensed under the MIT License.

from ._scenepic import Video


def load_from_buffer(self, data: bytes, ext: str):
    self.data = list(data)
    self.ext = ext


def _data_get(self) -> bytes:
    return bytes(self.data_get_)


def _data_set(self, data: bytes):
    self.data_set_(list(data))


Video.load_from_buffer = load_from_buffer
Video.load_from_buffer.__doc__ = """Load video from a pre-existing data buffer and extension.

Args:
    data (bytes): the binary data for the video
    ext (str): the file extension (e.g. mp4, mkv)
"""

Video.data = property(_data_get, _data_set, doc="""The binary video data.""")
