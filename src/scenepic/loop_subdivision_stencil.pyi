import numpy as np


class LoopSubdivisionStencil:
    """This class enables efficient Loop subdivision of triangle meshes.

    The subdivision stencil specifies what linear combinations of existing vertices are needed, and
    stencils can be reused across meshes with different vertex positions but identical triangle topologies.
    Thus, the constructor only takes the triangles, and caches the stencil.
    The stencil can be applied to a set of vertices using the apply() method.
    """

    def apply(self, vertices: np.ndarray) -> np.ndarray:
        """Applies the stencil to a vertex buffer.

        Description:
            This method takes a set of vertices and subdivides them (using the stencil) to
            create the vertices of a more detailed mesh.

        Args:
            vertices (np.ndarray):  a (N, 3) buffer of vertices

        Returns:
            np.ndarray: a (S, 3) buffer of subdivided vertices
        """

    @property
    def triangles(self) -> np.ndarray:
        """The triangles of the subdivided mesh"""

    @property
    def vertex_count(self) -> int:
        """The number of vertices in the subdivided mesh."""

    @property
    def triangle_count(self) -> int:
        """The number of triangles in the subdivided mesh."""

    @staticmethod
    def create(triangles: np.ndarray, steps: int,
               project_to_limit: bool) -> "LoopSubdivisionStencil":
        """Creates a new stencil by subdividing the provided triangles.

        Args:
            triangles (np.ndarray): the initial triangle indices
            steps (int): specifies how many steps of subdivision to apply. Defaults to 1.
            project_to_limit (bool): specifies whether the vertices should be projected onto the
                                     limit surface in the final step of subdivision. Defaults to false.

        Return:
            LoopSubdivisionStencil
        """
