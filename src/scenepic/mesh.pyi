import numpy as np
from .camera import Camera
from .mesh_info import MeshInfo


class VertexBuffer:
    """Class which provides dictionary access to vertex buffer blocks.

    Description:
        The different parts of the vertex buffer can be accessed via a dictionary
        interface:

        - "pos": the vertex positions
        - "norm": the vertex normals
        - "rgb": the vertex colors (if present)
        - "uv": the vertex uvs (if present)
    """

    def __init__(self, values: np.ndarray):
        """Constructor.

        Args:
            values (np.ndarray): The raw vertex buffer values
        """

    @property
    def shape(self) -> tuple:
        """Shape of the entire vertex buffer."""

    def copy(self) -> "VertexBuffer":
        """Returns a copy of the vertex buffer."""

    def __getitem__(self, key: str) -> np.ndarray:
        """Returns a sub-section of the buffer given the key.

        Args:
            key (str): one of ["pos", "norm", "rgb", "uv"]

        Returns:
            np.ndarray: a subsection of the buffer
        """

    def __setitem__(self, key: str, data: np.ndarray):
        """Sets a subsection of the buffer specified by the key.

        Args:
            key (str): one of ["pos", "norm", "rgb", "uv"]
            data (np.ndarray): The new values to place in the buffer
        """


class Mesh:
    """The basic ScenePic mesh class, containing vertex, triangle, and line buffers.

    To allow for compatibility with Numpy, we use row major order, so each
    data point is stored as a row for efficiency. Vertex buffer contains
    float32 elements with structure: x,y,z, and the normal buffer
    contains elements: nx,ny,nz.

    If shared_color is set then per-vertex color [cr,cg,cb,ca] in the
    color buffer is omitted to save memory.

    If texture_id is None then per-vertex texture coordinates [tu, tv] in
    the uv buffer are omitted to save memory.

    Right handed coordinate system used throughout.

    Triangle winding order is counter clockwise.
    """

    def __init__(self, shared_color: np.ndarray, texture_id: str):
        """A Mesh created in this way will not be added to the scene.

        Args:
            shared_color (np.ndarray, optional): shared color for all objects in the mesh. Defaults to None.
            texture_id (str, optional): The ID of the Image to use as a texture. Defaults to None.
        """

    def count_vertices(self) -> int:
        """The number of vertices in the mesh."""

    @property
    def center_of_mass(self) -> np.ndarray:
        """The mean of mesh vertex positions."""

    def reverse_triangle_order(self) -> None:
        """Reverses the winding order of all triangles in this mesh.
        Useful when interoping with existing codebases that use opposite convention.
        """

    def apply_transform(self, transform: np.ndarray) -> None:
        """Apply a 3D homogeneous matrix transform (i.e. 4x4 matrix) to all vertices
            (and appropriately to the normals) in the Mesh.

        Args:
            transform (np.ndarray): a 3D homogeneous transform
        """

    def apply_rotation(self, transform: np.ndarray) -> None:
        """Apply a 3D homogeneous matrix rotation (i.e. 4x4 matrix) to all vertices
            (and appropriately to the normals) in the Mesh.

           Separated out from apply_transform() so that a matrix inverse is not required, for speed.

        Args:
            transform (np.ndarray): a 3D homogeneous rotation matrix
        """

    def get_transformed(self, transform: np.ndarray) -> "Mesh":
        """Get a transformed copy of this Mesh.

        Args:
            transform (np.ndarray): a 3D homogeneous transform

        Returns:
            Mesh: a transformed copy of this mesh
        """

    def get_rotated(self, transform: np.ndarray) -> "Mesh":
        """Get a rotated copy of this Mesh.

        Args:
            transform (np.ndarray): a 3D homogeneous rotation matrix

        Returns:
            Mesh: a rotated copy of this mesh
        """

    def append_mesh(self, mesh: "Mesh") -> None:
        """Append another mesh object to this mesh.

        Args:
            mesh (Mesh): the mesh to append
        """

    def add_triangle(self, color: np.ndarray,
                     p0: np.ndarray, p1: np.ndarray, p2: np.ndarray,
                     normal: np.ndarray, fill_triangles: bool, add_wireframe: bool,
                     uv0: np.ndarray, uv1: np.ndarray, uv2: np.ndarray) -> None:
        """Adds a triangle to the mesh.

        Args:
            color (np.ndarray, optional): required unless Mesh was constructed with shared_color argument. Defaults to None.
            p0 (np.ndarray, optional): the first vertex. Defaults to [0, 0, 0].
            p1 (np.ndarray, optional): the second vertex. Defaults to [1, 0, 0].
            p2 (np.ndarray, optional): the third vertex. Defaults to [0, 1, 0].
            normal (np.ndarray, optional): will be computed automatically if None. Defaults to None.
            fill_triangles (bool, optional): whether to fill the primitive. Defaults to True.
            add_wireframe (bool, optional): whether to add a wireframe outline. Defaults to False.             
            uv_0 (np.ndarray, optional): first UV coordinate (for UV meshes). Defaults to (0, 0).
            uv_1 (np.ndarray, optional): second UV coordinate (for UV meshes). Defaults to (1, 0).
            uv_2 (np.ndarray, optional): third UV coordinate (for UV meshes). Defaults to (0, 1).
        """

    def add_quad(self, color: np.ndarray,
                 p0: np.ndarray, p1: np.ndarray, p2: np.ndarray, p3: np.ndarray,
                 normal: np.ndarray, fill_triangles: bool, add_wireframe: bool, transform: np.ndarray,
                 uv_0: np.ndarray, uv_1: np.ndarray, uv_2: np.ndarray, uv_3: np.ndarray) -> None:
        """Adds a quad to the mesh.
        All points are assumed to lie on the same plane.

        Args:
            color (np.ndarray, optional): required unless Mesh was constructed with shared_color argument. Defaults to None.
            p0 (np.ndarray, optional): the first vertex. Defaults to [0, -0.5, -0.5].
            p1 (np.ndarray, optional): the second vertex. Defaults to [0, 0.5, -0.5].
            p2 (np.ndarray, optional): the third vertex. Defaults to [0, 0.5, 0.5].
            p3 (np.ndarray, optional): the fourth vertex. Defaults to [0, -0.5, 0.5].
            normal (np.ndarray, optional): will be computed automatically if None. Defaults to None.
            fill_triangles (bool, optional): whether to fill the primitive. Defaults to True.
            add_wireframe (bool, optional): whether to add a wireframe outline. Defaults to False.
            transform (np.ndarray, optional): 3D homogeneous transform matrix. Defaults to None.
            uv_0 (np.ndarray, optional): first UV coordinate (for UV meshes). Defaults to (0, 0).
            uv_1 (np.ndarray, optional): second UV coordinate (for UV meshes). Defaults to (1, 0).
            uv_2 (np.ndarray, optional): third UV coordinate (for UV meshes). Defaults to (1, 1).
            uv_3 (np.ndarray, optional): fourth UV coordinate (for UV meshes). Defaults to (0, 1).        
        """

    def add_image(self, origin: np.ndarray, x_axis: np.ndarray,
                  y_axis: np.ndarray, normal: np.ndarray,
                  uv_0: np.ndarray, uv_1: np.ndarray, uv_2: np.ndarray, uv_3: np.ndarray,
                  double_sided: bool, transform: np.ndarray) -> None:
        """Add an image as a textured quad to this Mesh.
        The Mesh must have a texture_id.

        Args:
            origin (np.ndarray, optional): bottom left corner of image. Defaults to (-0.5, -0.5, 0).
            x_axis (np.ndarray, optional): direction and length of the x-axis of the image in world space.
                                            Defaults to (1, 0, 0).
            y_axis (np.ndarray, optional): direction and length of the y-axis of the image in world space.
                                            Defaults to (0, 1, 0).
            normal (np.ndarray, optional): will be computed automatically if None. Defaults to None.
            uv_0 (np.ndarray, optional): the bottom left uv coordinate. Defaults to (0, 0).
            uv_1 (np.ndarray, optional): the top left uv coordinate. Defaults to (1, 0).
            uv_2 (np.ndarray, optional): the tp right uv coordinate. Defaults to (1, 1).
            uv_3 (np.ndarray, optional): the bottom right uv coordinate. Defaults to (0, 1).
            double_sided (bool, optional): whether the image should be shown on both sides. Defaults to True.
            transform (np.ndarray, optional): a 3D homogeneous transform matrix. Defaults to None.
        """

    def add_cube(self, color: np.ndarray, transform: np.ndarray,
                 fill_triangles: bool, add_wireframe: bool) -> None:
        """Add a unit diameter cube to this mesh.

        Args:
            color (np.ndarray, optional): required unless Mesh was constructed with shared_color argument. Defaults to None.
            transform (np.ndarray, optional): homogeneous 3D transform matrix. Defaults to None.
            fill_triangles (bool, optional): whether to fill the primitive. Defaults to True.
            add_wireframe (bool, optional): whether to add a wireframe outline. Defaults to False.
        """

    def add_thickline(self, color: np.ndarray, start_point: np.ndarray,
                      end_point: np.ndarray, start_thickness: float,
                      end_thickness: float, transform: np.ndarray,
                      fill_triangles: bool, add_wireframe: bool) -> None:
        """Add a thick line to this Mesh.

        Args:
            color (np.ndarray, optional): required unless Mesh was constructed with shared_color argument. Defaults to None.
            start_point (np.ndarray, optional): the start point of the line. Defaults to [-0.5, 0, 0].
            end_point (np.ndarray, optional): the end point of the line. Defaults to [0.5, 0, 0].
            start_thickness (float, optional): the start thickness of the line. Defaults to 0.1.
            end_thickness (float, optional): the end thickness of the line. Defaults to 0.1.
            transform (np.ndarray, optional): homogeneous 3D transform matrix. Defaults to None.
            fill_triangles (bool, optional): whether to fill the primitive. Defaults to True.
            add_wireframe (bool, optional): whether to add a wireframe outline. Defaults to False.
        """

    def add_cone(self, color: np.ndarray, transform: np.ndarray,
                 truncation_height: float, lat_count: int, long_count: int,
                 fill_triangles: bool, add_wireframe: bool) -> None:
        """Add a cone to this Mesh.
        The cone has base center at [0.5, 0., 0.] and apex at [-0.5, 0., 0.].

        Args:
            color (np.ndarray, optional): required unless Mesh was constructed with shared_color argument. Defaults to None.
            transform (np.ndarray, optional): homogeneous 3D transform. Defaults to None.
            truncation_height (float, optional): draws a truncated cone of this height. Defaults to 1.
            lat_count (int, optional): number of discrete samples in latitude. Defaults to 10.
            long_count (int, optional): number of discrete samples in longitude. Defaults to 10.
            fill_triangles (bool, optional): whether to fill the primitive. Defaults to True.
            add_wireframe (bool, optional): whether to add a wireframe outline. Defaults to False.
        """

    def add_coordinate_axes(self, length: float, thickness: float, transform: np.ndarray) -> None:
        """Add a set of coordinate axes to this Mesh.
        xyz axes map to RGB colors.

        Args:
            length (float, optional): length of axes. Defaults to 1.0.
            thickness (float, optional): thickness of axes. Defaults to 0.1.
            transform (np.ndarray, optional): homogeneous 3D transform. Defaults to None.
        """

    def add_camera_frustum(self, camera: Camera, color: np.ndarray, fov_y_degrees: float,
                           aspect_ratio: float, depth: float, thickness: float,
                           transform: np.ndarray) -> None:
        """Adds a camera frustum visualization to this mesh.

        You can either specify the camera parameters using a Camera object
        or manually via fov_y_degrees, aspect_ratio, and transform.

        Args:
            camera (Camera): the camera to use for the frustum
            color (np.ndarray): required unless Mesh was constructed with shared_color argument. Defaults to None.
            depth (float, optional): depth of the frustum. Defaults to 1.0.
            thickness (float, optional): thickness of the edges. Defaults to 0.02.
            fov_y_degrees (float, optional): vertical field of view in degrees. Defaults to 45.0.
            aspect_ratio (float, optional): aspect ratio of field of view (width/height). Defaults to 16/9.
            transform (np.ndarray, optional): homogeneous 3D transform. Defaults to None.
        """

    def add_camera_image(self, camera: Camera, depth: float) -> None:
        """Add the image plane for a camera into the scene.

        Args:
            camera (Camera, optional): the ScenePic Camera corresponding to the image. 
            depth (float, optional): depth of the frustum. Defaults to 1.0.
        """

    def add_disc(self, color: np.ndarray, transform: np.ndarray, segment_count: int,
                 fill_triangles: bool, add_wireframe: bool) -> None:
        """Add a x-axis aligned unit diameter disc to this Mesh.

        Args:
            color (np.ndarray, optional): required unless Mesh was constructed with shared_color argument. Defaults to None.
            transform (np.ndarray, optional): homogeneous 3D transform. Defaults to None.
            segment_count (int, optional): number of segments in the disc. Defaults to 10.
            fill_triangles (bool, optional): whether to fill the primitive. Defaults to True.
            add_wireframe (bool, optional): whether to add a wireframe outline. Defaults to False.
        """

    def add_cylinder(self, color: np.ndarray, transform: np.ndarray, segment_count: int,
                     fill_triangles: bool, add_wireframe: bool) -> None:
        """Add a x-axis aligned unit diameter cylinder to this Mesh.

        Args:
            color (np.ndarray, optional): required unless Mesh was constructed with shared_color argument. Defaults to None.
            transform (np.ndarray, optional): homogeneous 3D transform. Defaults to None.
            segment_count (int, optional): number of segments in the cylinder. Defaults to 10.
            fill_triangles (bool, optional): whether to fill the primitive. Defaults to True.
            add_wireframe (bool, optional): whether to add a wireframe outline. Defaults to False.
        """

    def add_sphere(self, color: np.ndarray, transform: np.ndarray,
                   fill_triangles: bool, add_wireframe: bool) -> None:
        """Add a default (ico) unit diameter sphere primitive to the mesh.

        Args:
            color (np.ndarray, optional): required unless Mesh was constructed with shared_color argument. Defaults to None.
            transform (np.ndarray, optional): homogeneous 3D transform. Defaults to None.
            fill_triangles (bool, optional): whether to fill the primitive. Defaults to True.
            add_wireframe (bool, optional): whether to add a wireframe outline. Defaults to False.
        """

    def add_icosphere(self, color: np.ndarray, transform: np.ndarray, steps: int,
                      fill_triangles: bool, add_wireframe: bool) -> None:
        """Add a unit diameter ico-sphere to this Mesh.

        Args:
            color (np.ndarray, optional): required unless Mesh was constructed with shared_color argument. Defaults to None.
            transform (np.ndarray, optional): homogeneous 3D transform. Defaults to None.
            steps (int, optional): number of subdivision steps to apply to the icosahedron base primitive. Defaults to 0.
            fill_triangles (bool, optional): whether to fill the primitive. Defaults to True.
            add_wireframe (bool, optional): whether to add a wireframe outline. Defaults to False.
        """

    def add_uv_sphere(self, color: np.ndarray, transform: np.ndarray,
                      lat_count: int, long_count: int,
                      fill_triangles: bool, add_wireframe: bool) -> None:
        """Add a unit diameter UV sphere to this Mesh.

        Description:
            If the mesh has a texture attached, this method will automatically create
            a set of vertex UVs which unwraps the sphere to map to the full UV space.

        Args:
            color (np.ndarray, optional): required unless Mesh was constructed with shared_color argument. Defaults to None.
            transform (np.ndarray, optional): homogeneous 3D transform. Defaults to None.
            lat_count (int, optional): number of discrete samples in latitude. Defaults to 10.
            long_count (int, optional): number of discrete samples in longitude. Defaults to 10.
            fill_triangles (bool, optional): whether to fill the primitive. Defaults to True.
            add_wireframe (bool, optional): whether to add a wireframe outline. Defaults to False.
        """

    def add_mesh(self, mesh_info: MeshInfo, transform: np.ndarray, reverse_triangle_order: bool,
                 fill_triangles: bool, add_wireframe: bool) -> None:
        """Add a triangle mesh to this ScenePic Mesh, with normals computed automatically.

        Args:
            mesh_info (MeshInfo): object containing the vertices, triangles, and uvs/colors.
            transform (np.ndarray, optional): homogeneous 3D transform. Defaults to None.
            reverse_triangle_order (bool, optional): whether to reverse the triangle winding order. Defaults to False.
            fill_triangles (bool, optional): whether to fill the primitive. Defaults to True.
            add_wireframe (bool, optional): whether to add a wireframe outline. Defaults to False.
        """

    def add_mesh_without_normals(self, vertices: np.ndarray, triangles: np.ndarray,
                                 colors: np.ndarray, uvs: np.ndarray,
                                 transform: np.ndarray, reverse_triangle_order: bool,
                                 fill_triangles: bool, add_wireframe: bool) -> None:
        """Add a triangle mesh to this ScenePic Mesh, with normals computed automatically.

        Args:
            vertices (np.ndarray): float32 matrix of [N, 3] vertex positions
            triangles (np.ndarray): uint32 matrix of [N, 3] triangle vertex indices
            colors (np.ndarray, optional): float32 matrix of [N, 3] per-vertex RGB color vectors. Defaults to None.
            uvs (np.ndarray, optional): float32 matrix of [N, 2] per-vertex UV coordinates. Defaults to None.
            transform (np.ndarray, optional): homogeneous 3D transform. Defaults to None.
            reverse_triangle_order (bool, optional): whether to reverse the triangle winding order. Defaults to False.
            fill_triangles (bool, optional): whether to fill the primitive. Defaults to True.
            add_wireframe (bool, optional): whether to add a wireframe outline. Defaults to False.
        """

    def add_mesh_with_normals(self, vertices: np.ndarray, normals: np.ndarray,
                              triangles: np.ndarray, colors: np.ndarray, uvs: np.ndarray,
                              transform: np.ndarray, reverse_triangle_order: bool,
                              fill_triangles: bool, add_wireframe: bool) -> None:
        """Add a triangle mesh to this ScenePic Mesh.

        Args:
            vertices (np.ndarray): float32 matrix of [N, 3] vertex positions
            normals (np.ndarray): float32 matrix of [N, 3] vertex normals
            triangles (np.ndarray): uint32 matrix of [N, 3] triangle vertex indices
            colors (np.ndarray, optional): float32 matrix of [N, 3] per-vertex RGB color vectors. Defaults to None.
            uvs (np.ndarray, optional): float32 matrix of [N, 2] per-vertex UV coordinates. Defaults to None.
            transform (np.ndarray, optional): homogeneous 3D transform. Defaults to None.
            reverse_triangle_order (bool, optional): whether to reverse the triangle winding order. Defaults to False.
            fill_triangles (bool, optional): whether to fill the primitive. Defaults to True.
            add_wireframe (bool, optional): whether to add a wireframe outline. Defaults to False.
        """

    def add_lines(self, start_points: np.ndarray, end_points: np.ndarray,
                  color: np.ndarray, transform: np.ndarray) -> None:
        """Add a line cloud to this Mesh.

        Args:
            start_points (np.ndarray): float32 matrix of [N, 3] line start positions (or [N, 6] positions with colors)
            end_points (np.ndarray): float32 matrix of [N, 3] line end positions (or [N, 6] positions with colors)
            color (np.ndarray, optional): if provided, will override any per-point colors stored in start_colors and end_colors.
                                            Defaults to None.
            transform (np.ndarray): homogeneous 3D transform. Defaults to None.
        """

    def enable_instancing(self, positions: np.ndarray, rotations: np.ndarray,
                          colors: np.ndarray) -> None:
        """Makes ScenePic render this Mesh with multiple instances, e.g. for point-cloud visualizations.
        Can be used to make point clouds, for example.
        The whole contents of the Mesh will be rendered multiple times.

        Args:
            positions (np.ndarray): float32 matrix of [N, 3] instance positions
            rotations (np.ndarray, optional): float32 matrix of [N, 4] per-point quaternion rotations. Defaults to None.
            colors (np.ndarray, optional): float32 matrix of [N, 3] colors. If provided, overrides a Mesh's shared_color
                                            or per-vertex color. Defaults to None.
        """

    @property
    def shared_color(self) -> np.ndarray:
        """Whole-mesh color (reduces memory requirements but makes Mesh monochrome)."""

    @property
    def texture_id(self) -> str:
        """Specifies the id of a ScenePic Image to be used as a texture in this Mesh."""

    @property
    def mesh_id(self) -> str:
        """A unique identifier for the Mesh"""

    @property
    def layer_id(self) -> str:
        """An identifier to allow several Meshes to be grouped together into a "layer".

        Layers should be used in conjunction with Canvas3D.set_layer_settings().
        """

    @property
    def double_sided(self) -> bool:
        """Whether to turn off back face culling and draw the Mesh's triangles as double sided."""

    @property
    def camera_space(self) -> bool:
        """Whether this Mesh is defined in camera space (cannot be moved in the ScenePic user interface) or world space (standard)."""

    @property
    def nn_texture(self) -> bool:
        """ Whether the shader should use nearest-neighbor texture interpolation."""

    @property
    def use_texture_alpha(self) -> bool:
        """Whether to use the alpha channel in the texture for transparency (only relevant for textured Meshes)."""

    @property
    def is_billbord(self) -> bool:
        """Draw this Mesh as a billboard (i.e. always facing the user) rather than rotating with the rest of the world."""

    @property
    def vertex_buffer(self) -> VertexBuffer:
        """The raw vertex buffer."""


class MeshUpdate:
    """A new mesh formed by updated an existing mesh."""

    @property
    def base_mesh_id(self) -> str:
        """The unique identifier for the base mesh"""

    @property
    def mesh_id(self) -> str:
        """The unique identifier of the newly created mesh."""

    @property
    def frame_index(self) -> int:
        """The position in the creation order for this update."""

    @property
    def vertex_buffer(self) -> VertexBuffer:
        """The raw vertex buffer."""

    def quantize(self, keyframe_index: int, fixed_point_range: float, keyframe_vertex_buffer: VertexBuffer):
        """Quantize the mesh update.

        Args:
            self (MeshUpdate): self reference
            keyframe_index (int): Index of the keyframe to use in quantizing this update
            fixed_point_range (float): The range to use for the fixed point representation.
            keyframe_vertex_buffer (VertexBuffer): The keyframe vertex buffer
        """

    def difference_range(self, vertex_buffer: VertexBuffer) -> float:
        """Returns the absolute range of values in the difference between this update and the buffer.

        Args:
            self (MeshUpdate): self reference
            vertex_buffer (VertexBuffer): the buffer to use in the comparison

        Return:
            float: the absolute range (from minimum to maximum) in the per-index difference between
                this update and the reference.
        """
