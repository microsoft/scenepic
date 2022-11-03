"""Module which extends the scenepic Canvas3D type."""

# Copyright (c) Microsoft Corporation.
# Licensed under the MIT License.

from typing import Union, Mapping

from ._scenepic import (
    Canvas2D,
    LayerSettings,
)


def set_layer_settings(self, layer_settings: Mapping[str, Union[dict, LayerSettings]]):
    updated_settings = {}
    for key, value in layer_settings.items():
        if isinstance(value, dict):
            if "visible" in value:
                print("`visible` keyword has been deprecated")
            else:
                updated_settings[key] = LayerSettings(**value)

    self.set_layer_settings_(updated_settings)


Canvas2D.set_layer_settings = set_layer_settings
Canvas2D.set_layer_settings.__doc__ = """Specify the visibilities and opacities of certain mesh layers.

Description:
    Each Mesh object can optionally be part of a user-identified layer
    (see Mesh constructor).
    Calling set_layer_settings will result in an additional drop-down
    selector in the ScenePic user interface.

Args:
    self (Canvas2D): the self reference
    layer_settings (Mapping[str, Union[dict, LayerSettings]]): a mapping from
        layer IDs to layer settings.
"""
