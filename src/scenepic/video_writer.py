"""Component that wraps FFMPEG to allow for the production of videos."""

# Copyright (c) Microsoft Corporation.
# Licensed under the MIT License.

from subprocess import PIPE, Popen, STDOUT
from typing import Tuple

import cv2
import numpy as np


class VideoWriter:
    """The VideoWriter class.

    Description:
        Provides a straightforward means of producing videos by providing a lightweight wrapper
        around the commonly used FFMPEG command-line tool. Is best used as a context
        manager as shown in the example below.

    Example:
        Provided the ffmpeg executable is installed on the system, the user can create a
        video in the following way:

        .. code-block:: python

            import cv2
            import numpy as np
            from scenepic import VideoWriter

            with VideoWriter("example_video.mp4", (256, 256)) as video:
                angles = np.linspace(0, 2 * np.pi, 60, endpoint=False)
                for angle in angles:
                    x = int(np.cos(angle) * 64 + 128)
                    y = int(np.sin(angle) * 64 + 128)
                    video.clear_frame()
                    cv2.circle(video.frame, (x, y), 16, (0, 0, 255), -1)
                    video.write_frame()

        This will result in the following video:

        .. image:: ../../ci/assets/example_video.gif
    """

    def __init__(self,
                 output_path: str,
                 frame_size: Tuple[int, int],
                 quality: int = None,
                 ffmpeg_path="ffmpeg",
                 background_color=(0, 0, 0),
                 rgb=False,
                 audio_path=None,
                 codec="libx264",
                 framerate=30,
                 text="",
                 text_color=(1, 1, 0),
                 font_scale=1):
        """Constructor.

        Args:
            output_path (str): The path to the output video file
            frame_size (Tuple[int, int]): The (width, height) of the frame in pixels
            quality (int, optional): The 'q' or 'crf' value driving the quality of the encoding.
                Defaults to None.
            ffmpeg_path (str, optional): The path to the FFMPEG executable. Default "ffmpeg".
            background_color (Tuple[float, float, float], optional): The (r, g, b) background
                color for the frame where 0.0 <= val <= 1.0. Defaults to (0, 0, 0).
            rgb (bool, optional): Whether the pixels are in RGB or BGR format. Defaults to False (bgr).
            audio_path (str, optional): Path to an audio file to demux with the video. Defaults to None.
            codec (str, optional): Codec to use for encoding the video. Defaults to "libx264".
            framerate (float, optional): Framerate for video
            text (str, optional): Text to burn into the frame
            text_color (Tuple[float, float, float], optional): Color for the text
            font_scale (float, optional): Scaling factor for the font.
                                          Defaults to 1.
        """

        self._output_path = output_path
        if codec == "libx264":
            self._codec_flags = ["-c:v", "libx264"]
            if quality is not None:
                self._codec_flags.extend(["-crf", str(quality)])
        else:
            self._codec_flags = ["-c:v", codec]
            if quality is not None:
                self._codec_flags.extend(["q", str(quality)])

        if audio_path:
            self._audio_flags = ["-i", audio_path]
        else:
            self._audio_flags = []

        self._framerate = framerate
        self._rgb = rgb
        self._frame_size = frame_size
        self._ffmpeg_path = ffmpeg_path
        self._process = None
        self._text = text
        self._frame = np.zeros((frame_size[1], frame_size[0], 3), np.uint8)

        self._background_color = self._to_color(background_color)
        self._text_color = self._to_color(text_color)
        self._font_scale = font_scale

    def _to_color(self, color: Tuple[float, float, float]) -> np.array:
        r, g, b = color
        assert 0 <= r <= 1 and 0 <= g <= 1 and 0 <= b <= 1
        if self._rgb:
            color = np.array([r, g, b])
        else:
            color = np.array([b, g, r])

        return (color * 255).astype(np.uint8)

    def __enter__(self):
        """Called when using the VideoWriter as a context manager."""
        self.start()
        return self

    def __exit__(self, type, value, traceback):  # noqa: A002
        """Called when using the VideoWriter as a context manager."""
        self.stop()
        return False

    def start(self):
        """Starts the video writing process.

        No need to call this method manually if using VideoWriter as a context manager.
        """
        self._process = Popen([self._ffmpeg_path, "-y", "-f", "image2pipe",
                               "-c:v", "ppm",
                               "-s", "{}x{}".format(*self._frame_size),
                               "-pix_fmt", "bgr24",
                               "-framerate", str(self._framerate),
                               "-i", "-",
                               *self._audio_flags,
                               *self._codec_flags,
                               "-pix_fmt", "yuv420p",
                               self._output_path],
                              stdin=PIPE, stderr=STDOUT)

    def stop(self):
        """Stops the video writing process and closes the video file.

        No need to call this method manually if using VideoWriter as a context manager.
        """
        self._process.stdin.flush()
        self._process.stdin.close()
        self._process.wait()

    @property
    def text(self) -> str:
        """Text to burn into the frame."""
        return self._text

    @text.setter
    def text(self, text: str):
        self._text = text

    @property
    def text_color(self) -> np.ndarray:
        """The color of the burned text."""
        return (self._text_color / 255).astype(np.float32)

    @text_color.setter
    def text_color(self, text_color: Tuple[float, float, float]):
        self._text_color = self._to_color(text_color)

    @property
    def frame(self) -> np.ndarray:
        """Returns the frame buffer as a (H, W, 3) uint8 numpy array."""
        return self._frame

    def clear_frame(self):
        """Clears the frame buffer, setting all pixels to the background color."""
        self._frame[:, :] = self._background_color

    def write_frame(self):
        """Write the frame buffer to the video."""
        if self.text:
            width, height = self._frame.shape[:2]
            size = min(width, height)
            font_scale = self._font_scale * size / 512
            thickness = max(1, int(2 * font_scale))
            self._frame = cv2.putText(self._frame, self._text,
                                      (50, height - 50),
                                      cv2.FONT_HERSHEY_SIMPLEX, font_scale,
                                      tuple(self._text_color.tolist()),
                                      thickness, cv2.LINE_AA)

        if self._rgb:
            cv2.cvtColor(self._frame, cv2.COLOR_RGB2BGR, self._frame)

        _, buffer = cv2.imencode(".PPM", self._frame)
        self._process.stdin.write(buffer)
