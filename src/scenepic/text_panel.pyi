class TextPanel:
    """Represents a ScenePic TextPanel UI component."""

    @property
    def title(self) -> str:
        """The title of the text panel."""

    @property
    def text(self) -> str:
        """The text within the text panel."""

    @property
    def input_text(self) -> str:
        """Text for the optional input textbox."""

    def clear_script(self) -> None:
        """Call this if you are dumping a ScenePic script in multiple parts.

        Use this for client-server interactions, or when chunking a large
        visualization into multiple files.
        """
