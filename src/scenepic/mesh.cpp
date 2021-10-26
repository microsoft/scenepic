// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

/** File containing all non-primitive creation methods for Mesh (see
 * primitives.cpp) */

#include "mesh.h"

#include "transforms.h"
#include "util.h"

#include <Eigen/Geometry>
#include <array>
#include <climits>
#include <exception>
#include <map>
#include <utility>

namespace scenepic
{
  Vector compute_triangle_normal(
    const Vector& pos0, const Vector& pos1, const Vector& pos2)
  {
    Vector p01 = pos1 - pos0;
    Vector p02 = pos2 - pos0;
    Vector normal = p01.cross(p02);
    return normal.normalized();
  }

  Mesh::Mesh(const Color& shared_color, const std::string& texture_id)
  : Mesh("")
  {
    this->shared_color(shared_color);
    this->texture_id(texture_id);
  }

  Mesh::Mesh(const std::string& mesh_id)
  : m_mesh_id(mesh_id),
    m_shared_color(Color::None()),
    m_texture_id(""),
    m_layer_id(""),
    m_camera_space(false),
    m_double_sided(false),
    m_is_billboard(false),
    m_is_label(false),
    m_nn_texture(true),
    m_use_texture_alpha(false),
    m_vertices(VertexBuffer::Zero(0, 6)),
    m_triangles(TriangleBuffer::Zero(0, 3)),
    m_lines(LineBuffer::Zero(0, 2))
  {
    bool vertex_colors = m_shared_color.is_none();
    bool vertex_uvs = !m_texture_id.empty();
    if (vertex_colors && vertex_uvs)
    {
      throw std::invalid_argument(
        "Cannot have both vertex colors and vertex UVs");
    }
    else if (vertex_colors)
    {
      m_vertices = VertexBuffer::Zero(0, 9);
    }
    else if (vertex_uvs)
    {
      m_vertices = VertexBuffer::Zero(0, 8);
    }
  }

  std::uint32_t Mesh::count_vertices() const
  {
    return static_cast<std::uint32_t>(m_vertices.rows());
  }

  Vector Mesh::center_of_mass() const
  {
    return m_vertices.leftCols(3).rowwise().mean();
  }

  void Mesh::reverse_triangle_order()
  {
    m_triangles.col(1).swap(m_triangles.col(2));
    m_vertices.block(0, 3, this->count_vertices(), 3) *= -1;
  }

  void Mesh::apply_transform(const Transform& transform)
  {
    WorldVectorMatrix positions(this->count_vertices(), 4);
    positions.leftCols(3) = m_vertices.leftCols(3);
    positions.col(3).setConstant(1.0);
    positions *= transform.transpose();
    m_vertices.leftCols(3) = positions.leftCols(3);

    auto norm_transform = transform.topLeftCorner(3, 3).inverse();
    this->vertex_normals() *= norm_transform;
    this->vertex_normals().rowwise().normalize();
  }

  void Mesh::apply_rotation(const Transform& transform)
  {
    auto rotation = transform.topLeftCorner(3, 3).transpose();
    this->vertex_positions() *= rotation;
    this->vertex_normals() *= rotation;
  }

  Mesh Mesh::get_transformed(const Transform& transform)
  {
    Mesh mesh = *this;
    mesh.apply_transform(transform);
    return mesh;
  }

  Mesh Mesh::get_rotated(const Transform& transform)
  {
    Mesh mesh = *this;
    mesh.apply_rotation(transform);
    return mesh;
  }

  void Mesh::append_mesh(const Mesh& mesh)
  {
    auto vert_offset = this->count_vertices();
    append_matrix(m_vertices, mesh.m_vertices);
    TriangleBuffer triangles = mesh.m_triangles.array() + vert_offset;
    LineBuffer lines = mesh.m_lines.array() + vert_offset;
    append_matrix(m_triangles, triangles);
    append_matrix(m_lines, lines);
  }

  void Mesh::add_triangle(
    const Color& color,
    const Vector& p0,
    const Vector& p1,
    const Vector& p2,
    const Vector& n,
    bool fill_triangles,
    bool add_wireframe,
    const UV& uv0,
    const UV& uv1,
    const UV& uv2)
  {
    this->check_instances();
    this->check_color(color);
    Vector normal = n;
    if (normal == VectorNone())
    {
      normal = compute_triangle_normal(p0, p1, p2);
    }

    std::uint32_t i0, i1, i2;
    if (m_texture_id.empty())
    {
      i0 = this->append_vertex(p0, normal, color);
      i1 = this->append_vertex(p1, normal, color);
      i2 = this->append_vertex(p2, normal, color);
    }
    else
    {
      i0 = this->append_vertex(p0, normal, uv0);
      i1 = this->append_vertex(p1, normal, uv1);
      i2 = this->append_vertex(p2, normal, uv2);
    }

    if (fill_triangles)
    {
      this->append_triangle(i0, i1, i2);
    }

    if (add_wireframe)
    {
      this->append_line(i0, i1);
      this->append_line(i1, i2);
      this->append_line(i2, i0);
    }
  }

  void Mesh::add_quad(
    const Color& color,
    const Vector& p0,
    const Vector& p1,
    const Vector& p2,
    const Vector& p3,
    const Vector& n,
    bool fill_triangles,
    bool add_wireframe,
    const Transform& transform,
    const UV& uv0,
    const UV& uv1,
    const UV& uv2,
    const UV& uv3)
  {
    this->check_instances();
    this->check_color(color);
    Vector normal = n;
    if (normal == VectorNone())
    {
      normal = compute_triangle_normal(p0, p1, p2);
    }

    Mesh m = Mesh("").shared_color(m_shared_color).texture_id(m_texture_id);

    std::uint32_t i0, i1, i2, i3;
    if (m_texture_id.empty())
    {
      i0 = m.append_vertex(p0, normal, color);
      i1 = m.append_vertex(p1, normal, color);
      i2 = m.append_vertex(p2, normal, color);
      i3 = m.append_vertex(p3, normal, color);
    }
    else
    {
      i0 = m.append_vertex(p0, normal, uv0);
      i1 = m.append_vertex(p1, normal, uv1);
      i2 = m.append_vertex(p2, normal, uv2);
      i3 = m.append_vertex(p3, normal, uv3);
    }

    if (fill_triangles)
    {
      m.append_triangle(i0, i1, i2);
      m.append_triangle(i0, i2, i3);
    }

    if (add_wireframe)
    {
      m.append_line(i0, i1);
      m.append_line(i1, i2);
      m.append_line(i2, i3);
      m.append_line(i3, i0);
    }

    if (!transform.isIdentity())
    {
      m.apply_transform(transform);
    }

    this->append_mesh(m);
  }

  void Mesh::add_image(
    const Vector& origin,
    const Vector& x_axis,
    const Vector& y_axis,
    const Vector& n,
    const UV& uv0,
    const UV& uv1,
    const UV& uv2,
    const UV& uv3,
    bool double_sided,
    const Transform& transform)
  {
    this->check_instances();
    if (m_texture_id.length() == 0)
    {
      throw std::logic_error("Must set mesh's texture_id property first");
    }

    Vector p0 = origin;
    Vector p1 = p0 + x_axis;
    Vector p2 = p1 + y_axis;
    Vector p3 = p0 + y_axis;
    Vector normal = n;
    if (normal == VectorNone())
    {
      normal = compute_triangle_normal(p0, p1, p2);
    }

    Mesh m = Mesh("").texture_id(m_texture_id);

    // Front face
    auto i0 = m.append_vertex(p0, normal, uv0);
    auto i1 = m.append_vertex(p1, normal, uv1);
    auto i2 = m.append_vertex(p2, normal, uv2);
    auto i3 = m.append_vertex(p3, normal, uv3);
    m.append_triangle(i0, i1, i2);
    m.append_triangle(i0, i2, i3);

    // Back face
    if (double_sided)
    {
      i0 = m.append_vertex(p0, -normal, uv0);
      i1 = m.append_vertex(p1, -normal, uv1);
      i2 = m.append_vertex(p2, -normal, uv2);
      i3 = m.append_vertex(p3, -normal, uv3);
      m.append_triangle(i0, i2, i1);
      m.append_triangle(i0, i3, i2);
    }

    if (!transform.isIdentity())
    {
      m.apply_transform(transform);
    }

    this->append_mesh(m);
  }

  void Mesh::add_mesh(
    const std::shared_ptr<MeshInfo>& mesh_info,
    const Transform& transform,
    bool reverse_triangle_order,
    bool fill_triangles,
    bool add_wireframe)
  {
    if (mesh_info->has_normals())
    {
      this->add_mesh_with_normals(
        mesh_info->position_buffer(),
        mesh_info->normal_buffer(),
        mesh_info->triangle_buffer(),
        mesh_info->color_buffer(),
        mesh_info->uv_buffer(),
        transform,
        reverse_triangle_order,
        fill_triangles,
        add_wireframe);
    }
    else
    {
      this->add_mesh_without_normals(
        mesh_info->position_buffer(),
        mesh_info->triangle_buffer(),
        mesh_info->color_buffer(),
        mesh_info->uv_buffer(),
        transform,
        reverse_triangle_order,
        fill_triangles,
        add_wireframe);
    }
  }

  VectorBuffer Mesh::compute_normals(
    const ConstVectorBufferRef& vertices,
    const ConstTriangleBufferRef& triangles,
    bool reverse_triangle_order)
  {
    // Calculate per-face normals for the entire mesh, and normalize them
    VectorBuffer per_face_normals(triangles.rows(), 3);
    for (auto index = 0; index < triangles.rows(); ++index)
    {
      const Triangle& triangle = triangles.row(index);
      const Vector& p0 = vertices.row(triangle(0));
      const Vector& p1 = vertices.row(triangle(1));
      const Vector& p2 = vertices.row(triangle(2));
      per_face_normals.row(index) = (p1 - p0).cross(p2 - p0);
    }

    per_face_normals.rowwise().normalize();

    if (reverse_triangle_order)
    {
      per_face_normals *= -1;
    }

    // For each triangle, add that triangle's normal to each vertex in the
    // triangle
    VectorBuffer normals = VectorBuffer::Zero(vertices.rows(), 3);
    for (auto index = 0; index < triangles.rows(); ++index)
    {
      const Triangle& triangle = triangles.row(index);
      const Vector& normal = per_face_normals.row(index);
      normals.row(triangle(0)) += normal;
      normals.row(triangle(1)) += normal;
      normals.row(triangle(2)) += normal;
    }

    normals.rowwise().normalize();

    return normals;
  }

  void Mesh::add_mesh_without_normals(
    const ConstVectorBufferRef& vertices,
    const ConstTriangleBufferRef& triangles,
    const ConstColorBufferRef& colors,
    const ConstUVBufferRef& uvs,
    const Transform& transform,
    bool reverse_triangle_order,
    bool fill_triangles,
    bool add_wireframe)
  {
    this->check_instances();

    auto normals = Mesh::compute_normals(vertices, triangles);

    this->add_mesh_with_normals(
      vertices,
      normals,
      triangles,
      colors,
      uvs,
      transform,
      reverse_triangle_order,
      fill_triangles,
      add_wireframe);
  }

  void Mesh::add_mesh_with_normals(
    const ConstVectorBufferRef& vertices,
    const ConstVectorBufferRef& normals,
    const ConstTriangleBufferRef& triangles,
    const ConstColorBufferRef& colors,
    const ConstUVBufferRef& uvs,
    const Transform& transform,
    bool reverse_triangle_order,
    bool fill_triangles,
    bool add_wireframe)
  {
    assert(vertices.rows() == normals.rows());
    this->check_instances();

    bool has_texture = m_texture_id.length() > 0;
    bool has_vertex_colors = m_shared_color.is_none();
    bool has_uvs = uvs.rows() > 0;

    if (!has_texture && has_uvs)
    {
      throw std::invalid_argument(
        "Must create Mesh with texture_id in order to use uvs");
    }

    if (has_vertex_colors && has_uvs)
    {
      throw std::invalid_argument(
        "The use of vertex colors and uvs together is not supported");
    }

    if (has_vertex_colors && colors.rows() == 0)
    {
      throw std::invalid_argument(
        "Per-vertex colors must be provided unless the mesh has a single "
        "color "
        "or texture map");
    }

    if (has_vertex_colors && colors.rows() != vertices.rows())
    {
      throw std::invalid_argument("Expecting per-vertex colors");
    }

    if (has_uvs && (uvs.rows() != vertices.rows()))
    {
      throw std::invalid_argument("Expecting per-vertex uvs stored in uvs");
    }

    Mesh m = Mesh("").shared_color(m_shared_color).texture_id(m_texture_id);
    if (has_uvs)
    {
      m.m_vertices = VertexBuffer(vertices.rows(), 8);
      m.vertex_positions() = vertices;
      m.vertex_normals() = normals;
      m.vertex_uvs() = uvs;
    }
    else if (has_vertex_colors)
    {
      m.m_vertices = VertexBuffer(vertices.rows(), 9);
      m.vertex_positions() = vertices;
      m.vertex_normals() = normals;
      m.vertex_colors() = colors;
    }
    else
    {
      m.m_vertices = VertexBuffer(vertices.rows(), 6);
      m.vertex_positions() = vertices;
      m.vertex_normals() = normals;
    }

    if (add_wireframe)
    {
      m.m_lines = LineBuffer(triangles.rows() * 3, 2);
      m.m_lines.topRows(triangles.rows()) = triangles.leftCols(2);
      m.m_lines.block(triangles.rows(), 0, triangles.rows(), 2) =
        triangles.block(0, 1, triangles.rows(), 2);
      m.m_lines.block(2 * triangles.rows(), 0, triangles.rows(), 1) =
        triangles.col(0);
      m.m_lines.block(2 * triangles.rows(), 1, triangles.rows(), 1) =
        triangles.col(2);
    }

    if (fill_triangles)
    {
      m.m_triangles = triangles;
      if (reverse_triangle_order)
      {
        m.m_triangles.col(1).swap(m.m_triangles.col(2));
      }
    }

    if (!transform.isIdentity())
    {
      m.apply_transform(transform);
    }

    this->append_mesh(m);
  }

  void Mesh::add_lines(
    const ConstVertexBufferRef& start_points,
    const ConstVertexBufferRef& end_points,
    const Color& color,
    const Transform& transform)
  {
    assert(start_points.rows() == end_points.rows());
    assert(start_points.cols() == end_points.cols());

    bool per_point_color = m_shared_color.is_none() && color.is_none();
    if (per_point_color && start_points.cols() != 6)
    {
      throw std::invalid_argument(
        "Expecting either single-color mesh, or shared color for whole set "
        "of "
        "lines, or per-point color stored in start_points and end_points");
    }

    Mesh m = Mesh("").shared_color(m_shared_color).texture_id(m_texture_id);
    const std::uint32_t num_lines =
      static_cast<const std::uint32_t>(start_points.rows());

    if (per_point_color)
    {
      m.m_vertices = VertexBuffer(num_lines * 2, 9);
      m.vertex_colors().topRows(num_lines) = start_points.rightCols(3);
      m.vertex_colors().bottomRows(num_lines) = end_points.rightCols(3);
    }
    else if (!color.is_none())
    {
      m.m_vertices = VertexBuffer(num_lines * 2, 9);
      m.vertex_colors().col(0).fill(color.r());
      m.vertex_colors().col(1).fill(color.g());
      m.vertex_colors().col(2).fill(color.b());
    }
    else
    {
      m.m_vertices = VertexBuffer(num_lines * 2, 6);
    }

    m.vertex_positions().topRows(num_lines) = start_points.leftCols(3);
    m.vertex_positions().bottomRows(num_lines) = end_points.leftCols(3);
    m.vertex_normals().col(0).fill(1);
    m.vertex_normals().col(1).fill(0);
    m.vertex_normals().col(2).fill(0);

    typedef Eigen::Array<std::uint32_t, Eigen::Dynamic, 1> LineInit;
    m.m_lines = LineBuffer(num_lines, 2);
    for (std::uint32_t row = 0; row < num_lines; ++row)
    {
      m.m_lines(row, 0) = row;
      m.m_lines(row, 1) = row + num_lines;
    }

    if (!transform.isIdentity())
    {
      m.apply_transform(transform);
    }

    this->append_mesh(m);
  }

  void Mesh::enable_instancing(
    const ConstVectorBufferRef& positions,
    const ConstQuaternionBufferRef& rotations,
    const ConstColorBufferRef& colors)
  {
    if (m_instance_buffer.rows())
    {
      std::cerr << "WARNING: multiple calls to enable_instancing will replace "
                   "existing instance_buffers."
                << std::endl;
    }

    m_instance_buffer_has_rotations = false;
    m_instance_buffer_has_colors = false;
    if (rotations.size() && colors.size())
    {
      assert(rotations.rows() == positions.rows());
      assert(colors.rows() == positions.rows());
      m_instance_buffer_has_rotations = true;
      m_instance_buffer_has_colors = true;
      m_instance_buffer = InstanceBuffer(positions.rows(), 10);
      m_instance_buffer.leftCols(3) = positions;
      m_instance_buffer.block(0, 3, positions.rows(), 4) = rotations;
      m_instance_buffer.rightCols(3) = colors;
    }
    else if (rotations.size())
    {
      assert(rotations.rows() == positions.rows());
      m_instance_buffer_has_rotations = true;
      m_instance_buffer = InstanceBuffer(positions.rows(), 7);
      m_instance_buffer.leftCols(3) = positions;
      m_instance_buffer.rightCols(4) = rotations;
    }
    else if (colors.size())
    {
      assert(colors.rows() == positions.rows());
      m_instance_buffer_has_colors = true;
      m_instance_buffer = InstanceBuffer(positions.rows(), 6);
      m_instance_buffer.leftCols(3) = positions;
      m_instance_buffer.rightCols(3) = colors;
    }
    else
    {
      m_instance_buffer = positions;
    }
  }

  JsonValue Mesh::definition_to_json() const
  {
    std::string data_type;
    JsonValue obj;

    obj["VertexBuffer"] = matrix_to_json(m_vertices);

    if (m_vertices.rows() < 0xFFFF)
    {
      TriangleShortBuffer triangles = m_triangles.cast<std::uint16_t>();
      LineShortBuffer lines = m_lines.cast<std::uint16_t>();
      obj["IndexBufferType"] = "UInt16";
      obj["TriangleBuffer"] = matrix_to_json(triangles);
      obj["LineBuffer"] = matrix_to_json(lines);
    }
    else
    {
      obj["IndexBufferType"] = "UInt32";
      obj["TriangleBuffer"] = matrix_to_json(m_triangles);
      obj["LineBuffer"] = matrix_to_json(m_lines);
    }

    if (!m_shared_color.is_none())
    {
      obj["PrimitiveType"] = "SingleColorMesh";
      obj["Color"] = matrix_to_json(m_shared_color);
    }
    else
    {
      obj["PrimitiveType"] = "MultiColorMesh";
    }

    if (m_texture_id.length() > 0)
    {
      obj["TextureId"] = m_texture_id;
      obj["NearestNeighborTexture"] = m_nn_texture;
      obj["UseTextureAlpha"] = m_use_texture_alpha;
    }

    if (m_instance_buffer.rows() > 0)
    {
      obj["InstanceBuffer"] = matrix_to_json(m_instance_buffer);
      obj["InstanceBufferHasRotations"] = m_instance_buffer_has_rotations;
      obj["InstanceBufferHasColors"] = m_instance_buffer_has_colors;
    }

    return obj;
  }

  JsonValue Mesh::to_json() const
  {
    JsonValue root;
    root["CommandType"] = "DefineMesh";
    root["MeshId"] = m_mesh_id;
    if (m_layer_id.empty())
    {
      root["LayerId"] = JsonValue::nullSingleton();
    }
    else
    {
      root["LayerId"] = m_layer_id;
    }
    root["DoubleSided"] = m_double_sided;
    root["Definition"] = this->definition_to_json();
    root["CameraSpace"] = m_camera_space;
    root["IsBillboard"] = m_is_billboard;
    root["IsLabel"] = m_is_label;
    return root;
  }

  std::uint32_t Mesh::append_vertex(
    const Vector& pos, const Vector& normal, const Color& color)
  {
    if (color.is_none())
    {
      return this->append_vertex(pos, normal);
    }

    std::uint32_t index = this->count_vertices();
    Vertex vertex(1, 9);
    vertex.segment(0, 3) = pos;
    vertex.segment(3, 3) = normal.normalized();
    vertex.segment(6, 3) = color;
    append_row(m_vertices, vertex);

    return index;
  }

  std::uint32_t
  Mesh::append_vertex(const Vector& pos, const Vector& normal, UV texture_uv)
  {
    std::uint32_t index = this->count_vertices();
    Vertex vertex(1, 8);
    vertex.segment(0, 3) = pos;
    vertex.segment(3, 3) = normal.normalized();
    vertex.segment(6, 2) = texture_uv;
    append_row(m_vertices, vertex);
    return index;
  }

  std::uint32_t Mesh::append_vertex(const Vector& pos, const Vector& normal)
  {
    std::uint32_t index = this->count_vertices();
    Vertex vertex(1, 6);
    vertex.segment(0, 3) = pos;
    vertex.segment(3, 3) = normal.normalized();
    append_row(m_vertices, vertex);
    return index;
  }

  void Mesh::append_triangle(
    std::uint32_t index0, std::uint32_t index1, std::uint32_t index2)
  {
    append_row(m_triangles, Triangle(index0, index1, index2));
  }

  void Mesh::append_line(std::uint32_t index0, std::uint32_t index1)
  {
    append_row(m_lines, Line(index0, index1));
  }

  bool Mesh::is_instanced() const
  {
    return m_instance_buffer.rows() > 0;
  }

  void Mesh::check_instances()
  {
    if (this->is_instanced())
    {
      std::cerr
        << "WARNING: Editing Mesh after calling enable_instancing"
        << "(used for point/line clouds) can lead to unexpected results."
        << std::endl;
    }
  }

  void Mesh::check_color(const Color& color)
  {
    if (color.is_none() && m_shared_color.is_none())
    {
      throw std::invalid_argument("Expected a vertex color");
    }
  }

  const Color& Mesh::shared_color() const
  {
    return m_shared_color;
  }

  Mesh& Mesh::shared_color(const Color& shared_color)
  {
    if (m_shared_color.is_none() && !shared_color.is_none())
    {
      // remove per-vertex color
      m_vertices = m_vertices.leftCols(6);
    }
    else if (!m_shared_color.is_none() && shared_color.is_none())
    {
      // attempt to add per-vertex color.
      if (this->count_vertices() == 0)
      {
        m_vertices = VertexBuffer::Zero(0, 9);
      }
      else
      {
        // this behavior is undefined if vertices exist
        throw std::invalid_argument(
          "Cannot add per-vertex color to a non-empty shared color mesh");
      }
    }

    m_shared_color = shared_color;
    return *this;
  }

  const std::string& Mesh::texture_id() const
  {
    return m_texture_id;
  }

  Mesh& Mesh::texture_id(const std::string& texture_id)
  {
    m_texture_id = texture_id;
    if (!texture_id.empty())
    {
      // convert this to a UV mesh
      if (this->count_vertices() == 0)
      {
        m_shared_color = Color(1, 1, 1);
        m_vertices = VertexBuffer::Zero(0, 8);
      }
      else
      {
        // behavior not defined if vertices exist
        throw std::invalid_argument(
          "Cannot convert a per-vertex color mesh to a UV mesh");
      }
    }
    else
    {
      if (!m_shared_color.is_none())
      {
        // Remove the texture coordinates
        m_vertices = m_vertices.leftCols(6);
      }
    }

    return *this;
  }

  const std::string& Mesh::mesh_id() const
  {
    return m_mesh_id;
  }

  const std::string& Mesh::layer_id() const
  {
    return m_layer_id;
  }

  Mesh& Mesh::layer_id(const std::string& layer_id)
  {
    m_layer_id = layer_id;
    return *this;
  }

  bool Mesh::double_sided() const
  {
    return m_double_sided;
  }

  Mesh& Mesh::double_sided(bool double_sided)
  {
    m_double_sided = double_sided;
    return *this;
  }

  bool Mesh::camera_space() const
  {
    return m_camera_space;
  }

  Mesh& Mesh::camera_space(bool camera_space)
  {
    m_camera_space = camera_space;
    return *this;
  }

  bool Mesh::nn_texture() const
  {
    return m_nn_texture;
  }

  Mesh& Mesh::nn_texture(bool nn_texture)
  {
    m_nn_texture = nn_texture;
    return *this;
  }

  bool Mesh::use_texture_alpha() const
  {
    return m_use_texture_alpha;
  }

  Mesh& Mesh::use_texture_alpha(bool use_texture_alpha)
  {
    m_use_texture_alpha = use_texture_alpha;
    return *this;
  }

  bool Mesh::is_billboard() const
  {
    return m_is_billboard;
  }

  Mesh& Mesh::is_billboard(bool is_billboard)
  {
    m_is_billboard = is_billboard;
    return *this;
  }

  bool Mesh::is_label() const
  {
    return m_is_label;
  }

  Mesh& Mesh::is_label(bool is_label)
  {
    m_is_label = is_label;
    return *this;
  }

  const ConstTriangleBufferRef Mesh::triangles() const
  {
    return ConstTriangleBufferRef(m_triangles);
  }

  VertexBlock Mesh::vertex_positions()
  {
    return m_vertices.leftCols(3);
  }

  Mesh& Mesh::vertex_positions(const VertexBlock& vertices)
  {
    this->vertex_positions() = vertices;
    return *this;
  }

  const ConstVertexBlock Mesh::vertex_positions() const
  {
    return m_vertices.leftCols(3);
  }

  VertexBlock Mesh::vertex_normals()
  {
    return m_vertices.block(0, 3, this->count_vertices(), 3);
  }

  Mesh& Mesh::vertex_normals(const VertexBlock& normals)
  {
    this->vertex_normals() = normals;
    return *this;
  }

  const ConstVertexBlock Mesh::vertex_normals() const
  {
    return m_vertices.block(0, 3, this->count_vertices(), 3);
  }

  VertexBlock Mesh::vertex_colors()
  {
    return m_vertices.rightCols(3);
  }

  Mesh& Mesh::vertex_colors(const VertexBlock& colors)
  {
    this->vertex_colors() = colors;
    return *this;
  }

  const ConstVertexBlock Mesh::vertex_colors() const
  {
    return m_vertices.rightCols(3);
  }

  VertexBlock Mesh::vertex_uvs()
  {
    return m_vertices.rightCols(2);
  }

  Mesh& Mesh::vertex_uvs(const VertexBlock& uvs)
  {
    this->vertex_uvs() = uvs;
    return *this;
  }

  const ConstVertexBlock Mesh::vertex_uvs() const
  {
    return m_vertices.rightCols(2);
  }

  VertexBufferRef Mesh::vertex_buffer()
  {
    return VertexBufferRef(m_vertices);
  }

  InstanceBufferRef Mesh::instance_buffer()
  {
    return InstanceBufferRef(m_instance_buffer);
  }

  std::string Mesh::to_string() const
  {
    return this->to_json().to_string();
  }

} // namespace scenepic
