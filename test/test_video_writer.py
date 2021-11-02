"""Offline test script for VideoWriter."""

import argparse

import cv2
import numpy as np
from scenepic import VideoWriter


def _parse_args():
    parser = argparse.ArgumentParser("VideoWriter Test Script")
    parser.add_argument("--size", type=int, default=512,
                        help="Size of video frame")
    parser.add_argument("--num-frames", type=int, default=60,
                        help="Number of frames in the video.")
    return parser.parse_args()


def _main():
    args = _parse_args()
    ball_size = max(4, int(16 * args.size / 256))
    with VideoWriter("example_video.mp4", (args.size, args.size)) as video:
        angles = np.linspace(0, 2 * np.pi, args.num_frames, endpoint=False)
        red = np.linspace(1, 0, args.num_frames)
        green = np.linspace(1, 0.5, args.num_frames)
        blue = np.linspace(0, 1, args.num_frames)
        for angle, r, g, b in zip(angles, red, green, blue):
            x = int(np.cos(angle) * args.size / 4) + (args.size // 2)
            y = int(np.sin(angle) * args.size / 4) + (args.size // 2)
            video.clear_frame()
            cv2.circle(video.frame, (x, y), ball_size, (0, 0, 255), -1)
            video.text = "{:.2f}".format(angle)
            video.text_color = (r, g, b)
            video.write_frame()


if __name__ == "__main__":
    _main()
