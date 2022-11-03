"""Module which extends the scenepic AudioTrack type."""

# Copyright (c) Microsoft Corporation.
# Licensed under the MIT License.

from ._scenepic import AudioTrack


def load_from_buffer(self, data: bytes, ext: str):
    self.data = list(data)
    self.ext = ext


def _data_get(self):
    return bytes(self.data_get_)


def _data_set(self, data):
    self.data_set_(list(data))


AudioTrack.load_from_buffer = load_from_buffer
AudioTrack.load_from_buffer.__doc__ = """Load audio from a pre-existing data buffer and extension.

Args:
    self (AudioTrack): self reference
    data (bytes): the binary data for the audio
    ext (str): the file extension (e.g. mp3, ogg, wav)
"""

AudioTrack.data = property(_data_get, _data_set, doc="""The binary audio data.""")
