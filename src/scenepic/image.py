"""Module which extends the scenepic Image type."""

# Copyright (c) Microsoft Corporation.
# Licensed under the MIT License.

from io import BytesIO

import numpy as np
import PIL.Image

from ._scenepic import Image


def load_from_buffer(self, data: bytes, ext: str):
    self.data = list(data)
    self.ext = ext


def from_numpy(self, array: np.ndarray, format_str="png"):  # noqa: A002
    buff = BytesIO()
    if array.dtype.kind == "f":
        if array.max() <= 1.0:
            array *= 255

        array = array.astype(np.uint8)

    image = PIL.Image.fromarray(array)
    image.save(buff, format_str)
    self.data = list(buff.getvalue())
    self.ext = format_str
    buff.close()


def _data_get(self):
    return bytes(self.data_get_)


def _data_set(self, data):
    self.data_set_(list(data))


Image.load_from_buffer = load_from_buffer
Image.from_numpy = from_numpy
Image.data = property(_data_get, _data_set)
