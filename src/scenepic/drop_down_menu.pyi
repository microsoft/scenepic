from typing import List


class DropDownMenu:
    """Represents a ScenePic DropDownMenu UI component."""

    @property
    def title(self) -> str:
        """The title of the dropdown menu."""

    @property
    def items(self) -> List[str]:
        """The items in the dropdown menu."""

    @property
    def selection(self) -> str:
        """The currently selected item."""

    def set_disabled(self, index: int, disabled: bool) -> None:
        """Sets the disabled flag on an option.

        Args:
            index (int): index of the menu item
            disabled (bool): whether it should be disabled
        """

    def clear_script(self) -> None:
        """Call this if you are dumping a ScenePic script in multiple parts.

        Use this for client-server interactions, or when chunking a large
        visualization into multiple files.
        """
