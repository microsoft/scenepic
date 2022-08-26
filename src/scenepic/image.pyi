import numpy as np


class Image:
    """A ScenePic Image type"""

    def load(self, path: str) -> None:
        """Load an image file from the disk

        Args:
            path (str): the path to the image file
        """

    def load_from_buffer(self, data: bytes, ext: str):
        """Load an image from a pre-existing data buffer and extension.

        Args:
            self (Image): self reference
            data (bytes): the binary data for the image
            ext (str): the format of the image data [e.g. "gif", "jpg", "png"]
        """

    def from_numpy(self, array: np.ndarray, format_str: str):  # noqa: A002
        """Loads the image object from raw RGB values provided as a numpy array.

        Args:
            self (Image): self reference
            array (np.ndarray): The raw pixel RGB as either 0-255 bytes or 0-1 floats.
            format_str (str, optional): The format to use when compressing the image. Defaults to "png".
        """

    @property
    def image_id(self) -> str:
        """A unique identifier for the image"""

    @property
    def ext(self) -> str:
        """The extension of the image (e.g. JPG, PNG)"""

    @property
    def data(self) -> bytes:
        """The binary image data."""
