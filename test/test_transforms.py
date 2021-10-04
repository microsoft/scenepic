"""Tests for transform calculations."""

# Copyright (c) Microsoft Corporation.
# Licensed under the MIT License.

import numpy as np
import scenepic as sp


PI = np.array(np.pi, np.float32)
ANGLE = 60 * PI / 180
COS = np.cos(ANGLE)
SIN = np.sin(ANGLE)
VEC = np.array([0.1, 0.2, 0.3], np.float32)


def test_scale_1():
    expected = np.array([
        [0.1, 0, 0, 0],
        [0, 0.1, 0, 0],
        [0, 0, 0.1, 0],
        [0, 0, 0, 1]
    ], np.float32)

    np.testing.assert_allclose(sp.Transforms.scale(0.1), expected)


def test_scale_3():
    expected = np.array([
        [0.1, 0, 0, 0],
        [0, 0.2, 0, 0],
        [0, 0, 0.3, 0],
        [0, 0, 0, 1]
    ], np.float32)

    np.testing.assert_allclose(sp.Transforms.scale(VEC), expected)


def test_translate():
    expected = np.array([
        [1, 0, 0, 0.1],
        [0, 1, 0, 0.2],
        [0, 0, 1, 0.3],
        [0, 0, 0, 1]
    ], np.float32)

    np.testing.assert_allclose(sp.Transforms.translate(VEC), expected)


def test_rotation_about_x():
    expected = np.array([
        [1, 0, 0, 0],
        [0, COS, -SIN, 0],
        [0, SIN, COS, 0],
        [0, 0, 0, 1]
    ], np.float32)

    np.testing.assert_allclose(sp.Transforms.rotation_about_x(ANGLE), expected)


def test_rotation_about_y():
    expected = np.array([
        [COS, 0, SIN, 0],
        [0, 1, 0, 0],
        [-SIN, 0, COS, 0],
        [0, 0, 0, 1]
    ], np.float32)

    np.testing.assert_allclose(sp.Transforms.rotation_about_y(ANGLE), expected)


def test_rotation_about_z():
    expected = np.array([
        [COS, -SIN, 0, 0],
        [SIN, COS, 0, 0],
        [0, 0, 1, 0],
        [0, 0, 0, 1]
    ], np.float32)

    np.testing.assert_allclose(sp.Transforms.rotation_about_z(ANGLE), expected)


def test_euler_angles_to_matrix():
    expected = np.array([
        [0.936293, -0.289629, 0.198669, 0],
        [0.312992, 0.944703, -0.0978434, 0],
        [-0.159345, 0.153792, 0.97517, 0],
        [0, 0, 0, 1]
    ], np.float32)

    np.testing.assert_allclose(sp.Transforms.euler_angles_to_matrix(VEC), expected, rtol=0, atol=1e-6)


def test_rotation_to_align_x_to_axis():
    expected = np.array([
        [0.267261, -0.534522, -0.801784, 0],
        [0.534522, 0.774542, -0.338187, 0],
        [0.801784, -0.338187, 0.492719, 0],
        [0, 0, 0, 1]
    ], np.float32)

    np.testing.assert_allclose(sp.Transforms.rotation_to_align_x_to_axis(VEC), expected, rtol=0, atol=1e-6)


def test_quaternion_to_align_x_to_axis():
    expected = np.array([0, -0.503627, 0.335751, 0.796009], np.float32)

    np.testing.assert_allclose(sp.Transforms.quaternion_to_align_x_to_axis(VEC), expected, rtol=0, atol=1e-6)


def test_quaternion_to_matrix():
    quaternion = np.array([0, -0.503627, 0.335751, 0.796009], np.float32)
    expected = np.array([
        [0.267261, -0.534522, -0.801784, 0],
        [0.534522, 0.774542, -0.338187, 0],
        [0.801784, -0.338187, 0.492719, 0],
        [0, 0, 0, 1]
    ], np.float32)

    np.testing.assert_allclose(sp.Transforms.quaternion_to_matrix(quaternion), expected, rtol=0, atol=1e-5)


def test_gl_projection():
    fov_y_degrees = 45
    aspect_ratio = 16 / 9
    znear = 1
    zfar = 100
    expected = np.array([
        [1.357995, 0, 0, 0],
        [0, 2.414214, 0, 0],
        [0, 0, -1.020202, -2.020202],
        [0, 0, -1, 0],
    ], np.float32)

    actual = sp.Transforms.gl_projection(fov_y_degrees, aspect_ratio, znear, zfar)
    np.testing.assert_allclose(actual, expected, rtol=0, atol=1e-6)

    intrinsic = np.array([
        [7.5438633e+03, 1.1557838e+01, 2.0555000e+03],
        [0, 7.5612310e+03, 1.5035000e+03],
        [0, 0, 1]
    ], np.float32)
    width = 4112
    height = 3008
    expected = np.array([
        [3.6691942, -5.6215166e-03, 2.4319066e-04, 0],
        [0, 5.0274143, -3.3244680e-04, 0],
        [0, 0, -1.0202020, -2.0202019],
        [0, 0, -1, 0]
    ], np.float32)

    actual = sp.Transforms.gl_projection(intrinsic, width, height, znear, zfar)
    np.testing.assert_allclose(actual, expected, rtol=0, atol=1e-6)


def test_gl_world_to_camera():
    extrinsic = np.array([
        [0.6245465, 0.07013566, 0.77783203, -4.461637],
        [-0.12572001, -0.973942, 0.18876319, -0.7838287],
        [0.77080226, -0.21568045, -0.5994546, -4.4914584],
        [0, 0, 0, 1]
    ], np.float32)
    expected = np.array([
        [0.62454647, -0.12572001, 0.77080226, 6.149983],
        [-0.07013565, 0.9739419, 0.21568044, 1.4192035],
        [-0.77783203, -0.1887632, 0.5994546, -0.92593664],
        [0, 0, 0, 1]
    ], np.float32)

    np.testing.assert_allclose(sp.Transforms.gl_world_to_camera(extrinsic), expected, rtol=0, atol=1e-6)
