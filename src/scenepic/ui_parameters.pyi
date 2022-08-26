class UIParameters:
    """Per-Canvas3D parameters of the ScenePic user interface."""

    def __init__(self, pointer_alt_key_multiplier: float,
                 pointer_rotation_speed: float,
                 mouse_wheel_translation_speed: float,
                 layer_dropdown_visibility: str):
        """Constructor.

        Args:
            pointer_alt_key_multiplier (float, optional): when the Alt key is pressed, mouse movement
                                                          is scaled by this factor. Defaults to 0.2.
            pointer_rotation_speed (float, optional): linear scale factor of pointer translation
                                                      into camera rotation. Defaults to 0.01.
            mouse_wheel_translation_speed (float, optional): linear scale factor of mouse wheel input
                                                             into camera translation. Defaults to 0.005.
            layer_dropdown_visibility (str, optional): CSS visibility setting for the layer dropdown menu.
                                                       Defaults to "visible".
        """

    @property
    def pointer_alt_key_multiplier(self) -> float:
        """When the Alt key is pressed, mouse movement is scaled by this factor"""

    @property
    def pointer_rotation_speed(self) -> float:
        """Linear scale factor of pointer translation into camera rotation."""

    @property
    def mouse_wheel_translation_speed(self) -> float:
        """Linear scale factor of mouse wheel input into camera."""

    @property
    def layer_dropdown_visibility(self) -> float:
        """The CSS visibility setting for the layer dropdown menu."""
