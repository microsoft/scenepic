"""Module which extends the scenepic Video type."""

# Copyright (c) Microsoft Corporation.
# Licensed under the MIT License.

from ._scenepic import Video


def load_from_buffer(self, data: bytes, ext: str):
    """Load video from a pre-existing data buffer and extension.

    Args:
        self (Video): self reference
        data (bytes): the binary data for the video
        ext (str): the file extension (e.g. mp4, mkv)
    """
    self.data = list(data)
    self.ext = ext


def _data_get(self):
    return bytes(self.data_get_)


def _data_set(self, data):
    self.data_set_(list(data))


Video.load_from_buffer = load_from_buffer
Video.data = property(_data_get, _data_set, doc="The binary audio data")
