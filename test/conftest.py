"""PyTest configuration module."""

import base64
import json
import os
import zlib

import numpy as np
import pytest
from scenepic import Color


def _asset(name):
    if name.endswith(".json"):
        path = os.path.join(os.path.dirname(__file__), "assets", name)
        if os.path.exists(path):
            return path

    return os.path.join(os.path.dirname(__file__), "..", "ci", "assets", name)


def _decode_and_decompress(buffer: bytes) -> np.ndarray:
    buffer = base64.b64decode(buffer)
    buffer = zlib.decompress(buffer)
    return np.frombuffer(buffer, dtype=np.float32)


def _assert_buffer_equal(actual, expected):
    actual = _decode_and_decompress(actual)
    expected = _decode_and_decompress(expected)
    np.testing.assert_array_almost_equal(actual, expected)


def _assert_dict_equal(actual, expected):
    actual_keys = sorted(actual.keys())
    expected_keys = sorted(expected.keys())
    assert actual_keys == expected_keys

    for key in actual_keys:
        if key in ("VertexBuffer", "Transform", "LineBuffer", "R", "T", "Projection"):
            _assert_buffer_equal(actual[key], expected[key])
        else:
            _assert_item_equal(actual[key], expected[key])


def _assert_list_equal(actual, expected):
    for actual_i, expected_i in zip(actual, expected):
        _assert_item_equal(actual_i, expected_i)


def _assert_item_equal(actual, expected):
    assert isinstance(actual, type(expected))
    if isinstance(expected, list):
        _assert_list_equal(actual, expected)
    elif isinstance(expected, dict):
        _assert_dict_equal(actual, expected)
    else:
        assert actual == expected


def _assert_json_equal(actual, expected_path):
    """Assert that the json string provided is equal to the asset."""
    expected_path = _asset(expected_path + ".json")
    with open(expected_path) as file:
        expected = json.load(file)

    actual = json.loads(actual)

    _assert_item_equal(actual, expected)


@pytest.fixture(scope="module")
def color():
    return Color(0.83863144, 0.39671423, 0.77389568)


@pytest.fixture(scope="module")
def asset():
    return _asset


@pytest.fixture(scope="module")
def assert_json_equal():
    return _assert_json_equal
