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
