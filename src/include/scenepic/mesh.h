// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#ifndef _SCENEPIC_MESH_H_
#define _SCENEPIC_MESH_H_

#include "camera.h"
#include "color.h"
#include "json_value.h"
#include "matrix.h"
#include "mesh_info.h"

#include <array>
#include <cstdint>
#include <string>
#include <vector>

namespace scenepic
{
  /**
   * The basic ScenePic mesh class, containing vertex, triangle, and line
   * buffers. To allow for compatibility with Numpy, we use row major order, so
   * each data point is stored as a row for efficiency. Vertex buffer contains
   * float32 elements with structure: x,y,z, and the normal buffer
   * contains elements: nx,ny,nz.
   *
   * If shared_color is set then per-vertex color [cr,cg,cb,ca] in the
   * color buffer is omitted to save memory.
   *
   * If texture_id is None then per-vertex texture coordinates [tu, tv] in
   * the uv buffer are omitted to save memory.
   *
   * Right handed coordinate system used throughout.
   *
   * Triangle winding order is counter clockwise.
   */
  class Mesh
  {
  public:
    /** Constructor.
     *  A Mesh constructed in this way will not be added to
     *  a scene.
     *
     * \param shared_color the initial shared color for the mesh
     * \param texture_id the initial texture ID for the mesh
     */
    Mesh(
      const Color& shared_color = Color::None(),
      const std::string& texture_id = "");

    /** The number of vertices in the mesh. */
    std::uint32_t count_vertices() const;

    /** The mean of mesh vertex positions */
    Vector center_of_mass() const;

    /** Reverses the winding order of all triangles in this mesh.
     * Useful when interoping with existing codebases that use opposite
     * convention.
     */
    void reverse_triangle_order();

    /** Apply a 3D homogeneous matrix transform (i.e. 4x4 matrix) to all
     *  vertices (and appropriately to the normals) in the Mesh.
     *  \param transform a 3D homogeneous transform
     */
    void apply_transform(const Transform& transform);

    /** Apply a 3D homogeneous matrix rotation (i.e. 4x4 matrix) to all vertices
     *  (and appropriately to the normals) in the Mesh.
     *  Separated out from apply_transform() so that a matrix inverse is not
     *  required, for speed.
     *  \param transform a 3D homogeneous rotation matrix
     */
    void apply_rotation(const Transform& transform);

    /** Get a transformed copy of this Mesh.
     * \param transform a 3D homogeneous transform
     * \return a transformed copy of this mesh
     */
    Mesh get_transformed(const Transform& transform);

    /** Get a rotated copy of this Mesh.
     * \param transform a 3D homogeneous rotation matrix
     * \return a transformed copy of this mesh
     */
    Mesh get_rotated(const Transform& transform);

    /** Append another mesh object to this mesh.
     * \param mesh the mesh to append
     */
    void append_mesh(const Mesh& mesh);

    /** Adds a triangle to the mesh.
     * \param color required unless Mesh was constructed with shared_color
     *              argument.
     * \param pos0 the first vertex
     * \param pos1 the second vertex
     * \param pos2 the third vertex
     * \param normal will be computed automatically if set to zero
     * \param fill_triangles whether to fill the primitive
     * \param add_wireframe whether to add a wireframe outline
     * \param uv_0 optional first uv coordinate (for UV meshes)
     * \param uv_1 optional second uv coordinate (for UV meshes)
     * \param uv_2 optional third uv coordinate (for UV meshes)
     */
    void add_triangle(
      const Color& color = Color::None(),
      const Vector& pos0 = Vector(0, 0, 0),
      const Vector& pos1 = Vector(1, 0, 0),
      const Vector& pos2 = Vector(0, 1, 0),
      const Vector& normal = VectorNone(),
      bool fill_triangles = true,
      bool add_wireframe = false,
      const UV& uv_0 = UV(0, 0),
      const UV& uv_1 = UV(1, 0),
      const UV& uv_2 = UV(0, 1));

    /** Adds a quad to the mesh. All points are assumed to lie on the same
     *  plane.
     * \param color required unless Mesh was constructed with shared_color
     *              argument.
     * \param pos0 the first vertex
     * \param pos1 the second vertex
     * \param pos2 the third vertex
     * \param pos3 the fourth vertex
     * \param normal will be computed automatically if set to zero
     * \param fill_triangles whether to fill the primitive
     * \param add_wireframe whether to add a wireframe outline
     * \param transform optional transform
     * \param uv_0 optional first uv coordinate (for UV meshes)
     * \param uv_1 optional second uv coordinate (for UV meshes)
     * \param uv_2 optional third uv coordinate (for UV meshes)
     * \param uv_3 optional fourth uv coordinate (for UV meshes)
     */
    void add_quad(
      const Color& color = Color::None(),
      const Vector& pos0 = Vector(0, -0.5f, -0.5f),
      const Vector& pos1 = Vector(0, +0.5f, -0.5f),
      const Vector& pos2 = Vector(0, +0.5f, +0.5f),
      const Vector& pos3 = Vector(0, -0.5f, +0.5f),
      const Vector& normal = VectorNone(),
      bool fill_triangles = true,
      bool add_wireframe = false,
      const Transform& transform = Transform::Identity(),
      const UV& uv_0 = UV(0, 0),
      const UV& uv_1 = UV(1, 0),
      const UV& uv_2 = UV(1, 1),
      const UV& uv_3 = UV(0, 1));

    /** Add an image as a textured quad to this Mesh.
     * The Mesh must have a texture_id.
     * \param origin bottom left corner of image.
     * \param x_axis direction and length of the x-axis of the image in world
     *               space
     * \param y_axis direction and length of the y-axis of the image in world
     *               space
     * \param normal will be computed automatically if set to zero
     * \param uv_0 the bottom left UV coordinate
     * \param uv_1 the top left UV coordinate
     * \param uv_2 the top right UV coordinate
     * \param uv_3 the bottom right UV coordinate
     * \param double_sided whether the image should be shown on both sides
     * \param transform an optional transform
     */
    void add_image(
      const Vector& origin = Vector(-0.5f, -0.5f, 0),
      const Vector& x_axis = Vector(1, 0, 0),
      const Vector& y_axis = Vector(0, 1, 0),
      const Vector& normal = VectorNone(),
      const UV& uv_0 = UV(0, 0),
      const UV& uv_1 = UV(1, 0),
      const UV& uv_2 = UV(1, 1),
      const UV& uv_3 = UV(0, 1),
      bool double_sided = true,
      const Transform& transform = Transform::Identity());

    /** Add a unit diameter cube to this mesh.
     * \param color required unless Mesh was constructed with shared_color
     *              argument.
     * \param transform optional transform
     * \param fill_triangles whether to fill the primitive
     * \param add_wireframe whether to add a wireframe outline
     */
    void add_cube(
      const Color& color = Color::None(),
      const Transform& transform = Transform::Identity(),
      bool fill_triangles = true,
      bool add_wireframe = false);

    /** Add a thick line to this Mesh.
     * \param color required unless Mesh was constructed with shared_color
     *              argument.
     * \param start_point the start point of the line
     * \param end_point the end point of the line
     * \param start_thickness the start thickness of the line
     * \param end_thickness the end thickness of the line
     * \param transform optional transform
     * \param fill_triangles whether to fill the primitive
     * \param add_wireframe whether to add a wireframe outline
     */
    void add_thickline(
      const Color& color = Color::None(),
      const Vector& start_point = Vector(-0.5, 0, 0),
      const Vector& end_point = Vector(0.5, 0, 0),
      float start_thickness = 0.1f,
      float end_thickness = 0.1f,
      const Transform& transform = Transform::Identity(),
      bool fill_triangles = true,
      bool add_wireframe = false);

    /** Add a cone to this Mesh. The cone has base center at [0.5, 0., 0.]
     * and apex at [-0.5, 0., 0.].
     * \param color required unless Mesh was constructed with shared_color
     *              argument.
     * \param transform optional transform
     * \param truncation_height draws a truncated cone of this height
     * \param lat_count number of discrete samples in latitude
     * \param long_count number of discrete samples in longitude
     * \param fill_triangles whether to fill the primitive
     * \param add_wireframe whether to add a wireframe outline
     */
    void add_cone(
      const Color& color = Color::None(),
      const Transform& transform = Transform::Identity(),
      float truncation_height = 1,
      std::uint32_t lat_count = 10,
      std::uint32_t long_count = 10,
      bool fill_triangles = true,
      bool add_wireframe = false);

    /** Add a set of coordinate axes to this Mesh. xyz axes map to RGB colors.
     * \param length length of axes
     * \param thickness thickness of axes
     * \param transform optional transform
     */
    void add_coordinate_axes(
      float length = 1.0f,
      float thickness = 0.1f,
      const Transform& transform = Transform::Identity());

    /** Add a camera frustum visualization to this Mesh.
     * \param color required unless Mesh was constructed with shared_color
     *              argument.
     * \param fov_y_degrees vertical field of view in degrees
     * \param aspect_ratio aspect ratio of field of view (width/height)
     * \param depth depth of the frustrtum
     * \param thickness thickness of the edges
     * \param transform optional transform
     */
    void add_camera_frustum(
      const Color& color = Color::None(),
      float fov_y_degrees = 45.0f,
      float aspect_ratio = 16.0f / 9.0f,
      float depth = 1.0f,
      float thickness = 0.02f,
      const Transform& transform = Transform::Identity());

    /** Add a camera frustum visualization to this Mesh.
     *  \param camera the ScenePic Camera to visualize
     *  \param color required unless Mesh was constructed with shared_color
     *               argument.
     *  \param thickness thickness of the edges
     *  \param depth depth of the frustum
     */
    void add_camera_frustum(
      const Camera& camera,
      const Color& color = Color::None(),
      float thickness = 0.02f,
      float depth = 1.0f);

    /** Add the image plane for a camera into the scene.
     *  \param camera the ScenePic Camera corresponding to the image
     *  \param depth depth of the frustum
     */
    void add_camera_image(const Camera& camera, float depth = 1.0f);

    /** Add a x-axis aligned unit diameter disc to this Mesh.
     * \param color required unless Mesh was constructed with shared_color
     *              argument.
     * \param transform optional transform
     * \param segment_count number of segments in the disc
     * \param fill_triangles whether to fill the primitive
     * \param add_wireframe whether to add a wireframe outline
     */
    void add_disc(
      const Color& color = Color::None(),
      const Transform& transform = Transform::Identity(),
      std::uint32_t segment_count = 10,
      bool fill_triangles = true,
      bool add_wireframe = false);

    /** Add a x-axis aligned unit diameter cylinder to this Mesh.
     * \param color required unless Mesh was constructed with shared_color
     *              argument.
     * \param transform optional transform
     * \param segment_count number of segments in the cylinder
     * \param fill_triangles whether to fill the primitive
     * \param add_wireframe whether to add a wireframe outline
     */
    void add_cylinder(
      const Color& color = Color::None(),
      const Transform& transform = Transform::Identity(),
      std::uint32_t segment_count = 10,
      bool fill_triangles = true,
      bool add_wireframe = false);

    /** Add a default (ico) unit diameter sphere primitive to the mesh.
     * \param color required unless Mesh was constructed with shared_color
     *              argument.
     * \param transform optional transform
     * \param fill_triangles whether to fill the primitive
     * \param add_wireframe whether to add a wireframe outline
     */
    void add_sphere(
      const Color& color = Color::None(),
      const Transform& transform = Transform::Identity(),
      bool fill_triangles = true,
      bool add_wireframe = false);

    /** Add a unit diameter ico-sphere to this Mesh
     * \param color required unless Mesh was constructed with shared_color
     *              argument.
     * \param transform optional transform
     * \param steps number of subdivision steps to apply to the icosahedron
     *              base primitive
     * \param fill_triangles whether to fill the primitive
     * \param add_wireframe whether to add a wireframe outline
     */
    void add_icosphere(
      const Color& color = Color::None(),
      const Transform& transform = Transform::Identity(),
      std::uint32_t steps = 0,
      bool fill_triangles = true,
      bool add_wireframe = false);

    /** Add a unit diameter UV sphere to this Mesh. If the mesh has a texture
     *  attached, this method will automatically create a set of vertex UVs
     *  which unwraps the sphere to map to the full UV space.
     *  \param color required unless Mesh was constructed with shared_color
     *               argument.
     *  \param transform optional transform
     *  \param lat_count number of discrete samples in latitude
     *  \param long_count number of discrete samples in longitude
     *  \param fill_triangles whether to fill the primitive
     *  \param add_wireframe whether to add a wireframe outline
     */
    void add_uv_sphere(
      const Color& color = Color::None(),
      const Transform& transform = Transform::Identity(),
      std::uint32_t lat_count = 10,
      std::uint32_t long_count = 10,
      bool fill_triangles = true,
      bool add_wireframe = false);

    /** Add a triangle mesh to this ScenePic Mesh, with normals computed
     *  automatically.
     *  \param vertices matrix of N vertex positions
     *  \param triangles matrix of N triangle vertex indices
     *  \param colors optional matrix of N per-vertex RGB color vectors
     *  \param uvs optional matrix of N per-vertex UV coordinates
     *  \param transform optional transform
     *  \param reverse_triangle_order whether to reverse the triangle winding
     *                                order
     *  \param fill_triangles whether to fill the primitive
     *  \param add_wireframe whether to add a wireframe outline
     */
    void add_mesh_without_normals(
      const ConstVectorBufferRef& vertices,
      const ConstTriangleBufferRef& triangles,
      const ConstColorBufferRef& colors = ColorBufferNone(),
      const ConstUVBufferRef& uvs = UVBufferNone(),
      const Transform& transform = Transform::Identity(),
      bool reverse_triangle_order = false,
      bool fill_triangles = true,
      bool add_wireframe = false);

    /** Compute the vertex normals given a set of triangles and vertices.
     *
     *  \param vertices the vertex positions
     *  \param triangles the triangles defining the mesh
     *  \param reverse_triangle_order whether to reverse the triangle order
     *                                when computing the normals.
     *  \return a buffer containing the normalized per-vertex normals
     */
    static VectorBuffer compute_normals(
      const ConstVectorBufferRef& vertices,
      const ConstTriangleBufferRef& triangles,
      bool reverse_triangle_order = false);

    /** Add a triangle mesh to this ScenePic Mesh, with normals provided.
     * \param vertices matrix of N vertex positions
     * \param normals matrix of N vertex normals
     * \param triangles matrix of N triangle vertex indices
     * \param colors optional matrix of N per-vertex RGB color vectors
     * \param uvs optional matrix of N per-vertex UV coordinates
     * \param transform optional transform
     * \param reverse_triangle_order whether to reverse the triangle winding
     *                               order
     * \param fill_triangles whether to fill the primitive
     * \param add_wireframe whether to add a wireframe outline
     */
    void add_mesh_with_normals(
      const ConstVectorBufferRef& vertices,
      const ConstVectorBufferRef& normals,
      const ConstTriangleBufferRef& triangles,
      const ConstColorBufferRef& colors = ColorBufferNone(),
      const ConstUVBufferRef& uvs = UVBufferNone(),
      const Transform& transform = Transform::Identity(),
      bool reverse_triangle_order = false,
      bool fill_triangles = true,
      bool add_wireframe = false);

    /** Add a triangle mesh to this ScenePic Mesh, with normals computed
     *  automatically.
     *  \param mesh_info the positions, triangles and other elements of the mesh
     *  \param transform optional transform
     *  \param reverse_triangle_order whether to reverse the triangle
     *                                winding order
     *  \param fill_triangles whether to fill the primitive
     *  \param add_wireframe whether to add a wireframe outline
     */
    void add_mesh(
      const std::shared_ptr<MeshInfo>& mesh_info,
      const Transform& transform = Transform::Identity(),
      bool reverse_triangle_order = false,
      bool fill_triangles = true,
      bool add_wireframe = false);

    /** Add a line cloud to this Mesh.
     * \param start_points defines N line start positions (and, optionally,
     *                     the per-point colors)
     * \param end_points defines N line end positions (and, optionally, the
     *                   per-point colors)
     * \param color if provided, will override any per-point colors stored in
     *              start_colors and end_colors.
     * \param transform optional transform
     */
    void add_lines(
      const ConstVertexBufferRef& start_points,
      const ConstVertexBufferRef& end_points,
      const Color& color = Color::None(),
      const Transform& transform = Transform::Identity());

    /** Makes ScenePic render this Mesh with multiple instances, e.g. for
     *  point-cloud visualizations. Can be used to make point clouds, for
     *  example. The whole contents of the Mesh will be rendered multiple times.
     * \param positions the positions of the N instances
     * \param colors if provided, a per-instance color that overrides a
     *               Mesh's shared_color or per-vertex color
     * \param rotations if provided, a per-instance quaternion rotation that
     *                  rotates each instance
     */
    void enable_instancing(
      const ConstVectorBufferRef& positions,
      const ConstQuaternionBufferRef& rotations = QuaternionBufferNone(),
      const ConstColorBufferRef& colors = ColorBufferNone());

    /** Return a JSON string representing the object */
    std::string to_string() const;

    /** Convert this mesh into ScenePic json.
     * \return a json value
     */
    JsonValue to_json() const;

    /** Whole-mesh color (reduces memory requirements but makes Mesh
     * monochrome). */
    const Color& shared_color() const;

    /** Whole-mesh color (reduces memory requirements but makes Mesh
     * monochrome). */
    Mesh& shared_color(const Color& shared_color);

    /** Specifies the id of a ScenePic Image to be used as a texture in this
     * Mesh. */
    const std::string& texture_id() const;

    /** Specifies the id of a ScenePic Image to be used as a texture in this
     * Mesh. */
    Mesh& texture_id(const std::string& texture_id);

    /** A unique identifier for the Mesh */
    const std::string& mesh_id() const;

    /** An identifier to allow several Meshes to be grouped
     *  together into a "layer".  Layers should be used in
     *  conjunction with Canvas3D.set_layer_settings().
     *  \see Canvas3D::set_layer_settings.
     */
    const std::string& layer_id() const;

    /** An identifier to allow several Meshes to be grouped
     *  together into a "layer".  Layers should be used in
     *  conjunction with Canvas3D.set_layer_settings().
     *  \see Canvas3D::set_layer_settings.
     */
    Mesh& layer_id(const std::string& layer_id);

    /** Whether to turn off back face culling and draw
     *  the Mesh's triangles as double sided.
     */
    bool double_sided() const;

    /** Whether to turn off back face culling and draw
     *  the Mesh's triangles as double sided.
     */
    Mesh& double_sided(bool double_sided);

    /** Whether this Mesh is defined in camera space
     *  (cannot be moved in the ScenePic user interface) or
     *  world space (standard).
     */
    bool camera_space() const;

    /** Whether this Mesh is defined in camera space
     *  (cannot be moved in the ScenePic user interface) or
     *  world space (standard).
     */
    Mesh& camera_space(bool camera_space);

    /** Whether the shader should use nearest-neighbor
     *  texture interpolation.
     */
    bool nn_texture() const;

    /** Whether the shader should use nearest-neighbor
     *  texture interpolation.
     */
    Mesh& nn_texture(bool nn_texture);

    /** Whether to use the alpha channel in the
     *  texture for transparency (only relevant
     *  for textured Meshes).
     */
    bool use_texture_alpha() const;

    /** Whether to use the alpha channel in the
     *  texture for transparency (only relevant
     *  for textured Meshes).
     */
    Mesh& use_texture_alpha(bool use_texture_alpha);

    /** Draw this Mesh as a billboard (i.e. always
     *  facing the user) rather than rotating with the
     *  rest of the world.
     */
    bool is_billboard() const;

    /** Draw this Mesh as a billboard (i.e. always
     *  facing the user) rather than rotating with the
     *  rest of the world.
     */
    Mesh& is_billboard(bool is_billboard);

    /** This mesh will be treated specially as a label.
     *  Not for public use.
     */
    bool is_label() const;

    /** This mesh will be treated specially as a label.
     *  Not for public use.
     */
    Mesh& is_label(bool is_label);

    /** The triangles (i.e. face vertex indices) defining the mesh */
    const ConstTriangleBufferRef triangles() const;

    /** Reference to the vertex positions. */
    VertexBlock vertex_positions();

    /** const Reference to the vertex positions. */
    const ConstVertexBlock vertex_positions() const;

    /** Set the vertex positions */
    Mesh& vertex_positions(const VertexBlock& vertices);

    /** Reference to the vertex normals. */
    VertexBlock vertex_normals();

    /** Set the vertex normals */
    Mesh& vertex_normals(const VertexBlock& normals);

    /** const Reference to the vertex normals. */
    const ConstVertexBlock vertex_normals() const;

    /** Reference to the vertex colors. */
    VertexBlock vertex_colors();

    /** Set the vertex colors */
    Mesh& vertex_colors(const VertexBlock& colors);

    /** const Reference to the vertex colors. */
    const ConstVertexBlock vertex_colors() const;

    /** Reference to the vertex uvs. */
    VertexBlock vertex_uvs();

    /** Set the vertex uvs */
    Mesh& vertex_uvs(const VertexBlock& uvs);

    /** const Reference to the vertex uvs. */
    const ConstVertexBlock vertex_uvs() const;

    /** Reference to the vertex buffer.
     *  Vertices are stored in a NxD matrix in the following way per row:
     *  [x, y, z, nx, ny, nz] (no color or uvs)
     *  [x, y, z, nx, ny, nz, r, g, b] (+ color)
     *  [x, y, z, nx, ny, nz, u, v] (+ uv)
     */
    VertexBufferRef vertex_buffer();

    /** References to the instance buffer.
     *  Instances are stored in a NxD matrix in the following way per row:
     *  [x, y, z]
     *  [x, y, z, r, g, b] (+ color)
     *  [x, y, z, qx, qy, qz, qw] (+ rotation)
     *  [x, y, z, qx, qy, qz, qw, r, g, b] (+ rotation and color)
     */
    InstanceBufferRef instance_buffer();

  private:
    friend class Scene;

    /** Constructor.
     * \param mesh_id a unique identifier for the Mesh
     */
    Mesh(const std::string& mesh_id);

    bool is_instanced() const;
    void check_instances();
    void check_color(const Color& color);
    std::uint32_t
    append_vertex(const Vector& pos, const Vector& normal, const Color& color);
    std::uint32_t
    append_vertex(const Vector& pos, const Vector& normal, UV texture_uv);
    std::uint32_t append_vertex(const Vector& pos, const Vector& normal);
    void append_triangle(
      std::uint32_t index0, std::uint32_t index1, std::uint32_t index2);
    void append_line(std::uint32_t index0, std::uint32_t index1);
    JsonValue definition_to_json() const;

    VertexBuffer m_vertices;
    TriangleBuffer m_triangles;
    LineBuffer m_lines;
    Color m_shared_color;
    std::string m_texture_id;
    std::string m_mesh_id;
    std::string m_layer_id;
    bool m_double_sided;
    bool m_camera_space;
    bool m_nn_texture;
    bool m_use_texture_alpha;
    bool m_is_billboard;
    bool m_is_label;

    InstanceBuffer m_instance_buffer;
    bool m_instance_buffer_has_rotations;
    bool m_instance_buffer_has_colors;
  };
} // namespace scenepic

#endif