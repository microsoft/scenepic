class LayerSettings:
    """Settings used for customizing canvas drawing by layer.
    
    Currently, opacity is only guaranteed to be correct for Meshes
    that do not overlap.
    If you know one mesh should be rendered before another for opacity purposes,
    use the renderOrder setting: low numbers will be drawn before high numbers,
    and layers with no renderOrder set will be drawn last. Meshes without layer ids,
    or with layer ids that are not specified in this dictionary, will be drawn filled,
    without wireframe, at opacity 1.0, and after any layers with renderOrder set.
    """

    def __init__(self, filled: bool, wireframe: bool,
                 opacity: float, render_order: int):
        """Constructor.

        Args:
            filled (bool, optional): whether to draw the layer with filled polygons. Defaults to True.
            wireframe (bool, optional): whether to draw the layer with a frame outline. Defaults to False.
            opacity (float, optional): the opacity with which to draw the layer. Defaults to 1.0.
            render_order (int, optional): Where in the transparency order to draw this layer. Defaults to None.
        """

    @property
    def filled(self) -> bool:
        """Specifies whether to draw the layer with filled triangles."""

    @property
    def wireframe(self) -> bool:
        """Specifies whether to draw the layer with a wireframe outline."""

    @property
    def opacity(self) -> float:
        """In [0,1] specifies the opacity with which to draw the layer."""

    @property
    def render_order(self) -> int:
        """Specifies a fixed order to draw transparency."""
