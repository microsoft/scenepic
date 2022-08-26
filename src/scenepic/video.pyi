class Video:
    """A ScenePic Video type."""

    def load(self, path: str) -> None:
        """Load a video file from the disk.

        Args:
            path (str): the path to the video file
        """

    def load_from_buffer(self, data: bytes, ext: str) -> None:
        """Load video from a pre-existing data buffer and extension.

        Args:
            data (bytes): the binary data for the video
            ext (str): the file extension (e.g. mp4, mkv)
        """

    @property
    def audio_id(self) -> str:
        """A unique identifier for the video."""

    @property
    def data(self) -> bytes:
        """The binary video data."""
