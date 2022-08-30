class AudioTrack:
    """A ScenePic AudioTrack type."""

    def load(self, path: str) -> None:
        """Load an audio file from the disk.

        Args:
            path (str): the path to the audio file
        """

    def load_from_buffer(self, data: bytes, ext: str) -> None:
        """Load audio from a pre-existing data buffer and extension.

        Args:
            self (AudioTrack): self reference
            data (bytes): the binary data for the audio
            ext (str): the file extension (e.g. mp3, ogg, wav)
        """

    @property
    def video_id(self) -> str:
        """A unique identifier for the audio track."""

    @property
    def data(self) -> bytes:
        """The binary audio data."""
