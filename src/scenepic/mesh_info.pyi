import numpy as np


class MeshInfo:
    """Holds information needed to construct a mesh object.
    Includes information like the vertex positions, the triangulation,
    and either per-vertex UV coordinates or Color (if present).
    """

    @property
    def positions(self) -> np.ndarray:
        """The position buffer."""

    @property
    def normals(self) -> np.ndarray:
        """The normal buffer."""

    @property
    def triangles(self) -> np.ndarray:
        """The triangle buffer."""

    @property
    def uvs(self) -> np.ndarray:
        """The UV buffer."""

    @property
    def colors(self) -> np.ndarray:
        """The color buffer."""

    @property
    def has_normals(self) -> bool:
        """Whether the mesh info contains vertex normals."""

    @property
    def subdivide(self, steps: int, project_to_limit: bool) -> "MeshInfo":
        """Subdivide this mesh using loop subdivision.

        Args:
            steps (int): specifies how many steps of subdivision to apply. Defaults to 1.
            project_to_limit (bool): specifies whether the vertices should be projected onto the
                                     limit surface in the final step of subdivision. Defaults to false.

        Returns:
            MeshInfo: a subdivided version of this mesh
        """
