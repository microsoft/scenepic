// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#define _USE_MATH_DEFINES
#include "mesh.h"
#include "transforms.h"

#include <Eigen/Geometry>
#include <cmath>

namespace
{
  const float PI = static_cast<float>(M_PI);
}

namespace scenepic
{
  Vector interpolate(const Vector& p0, const Vector& p1, float alpha)
  {
    return alpha * p0 + (1. - alpha) * p1;
  }

  void Mesh::add_cube(
    const Color& color,
    const Transform& transform,
    bool fill_triangles,
    bool add_wireframe)
  {
    this->check_instances();
    this->check_color(color);
    Vector p0(-0.5, -0.5, -0.5);
    Vector p1(+0.5, -0.5, -0.5);
    Vector p2(-0.5, +0.5, -0.5);
    Vector p3(+0.5, +0.5, -0.5);
    Vector p4(-0.5, -0.5, +0.5);
    Vector p5(+0.5, -0.5, +0.5);
    Vector p6(-0.5, +0.5, +0.5);
    Vector p7(+0.5, +0.5, +0.5);

    Mesh m = Mesh("").shared_color(m_shared_color).texture_id(m_texture_id);
    m.add_quad(
      color, p0, p2, p3, p1, VectorNone(), fill_triangles, add_wireframe);
    m.add_quad(
      color, p1, p3, p7, p5, VectorNone(), fill_triangles, add_wireframe);
    m.add_quad(
      color, p5, p7, p6, p4, VectorNone(), fill_triangles, add_wireframe);
    m.add_quad(
      color, p4, p6, p2, p0, VectorNone(), fill_triangles, add_wireframe);
    m.add_quad(
      color, p2, p6, p7, p3, VectorNone(), fill_triangles, add_wireframe);
    m.add_quad(
      color, p4, p0, p1, p5, VectorNone(), fill_triangles, add_wireframe);

    if (!transform.isIdentity())
    {
      m.apply_transform(transform);
    }

    this->append_mesh(m);
  }

  void Mesh::add_thickline(
    const Color& color,
    const Vector& start_point,
    const Vector& end_point,
    float start_thickness,
    float end_thickness,
    const Transform& transform_init,
    bool fill_triangles,
    bool add_wireframe)
  {
    this->check_instances();
    this->check_color(color);

    // For convenience
    float length = (end_point - start_point).norm() * 0.5f;
    start_thickness *= 0.5f;
    end_thickness *= 0.5f;

    // Create points
    Vector p0(-length, -start_thickness, -start_thickness);
    Vector p1(+length, -end_thickness, -end_thickness);
    Vector p2(-length, +start_thickness, -start_thickness);
    Vector p3(+length, +end_thickness, -end_thickness);
    Vector p4(-length, -start_thickness, +start_thickness);
    Vector p5(+length, -end_thickness, +end_thickness);
    Vector p6(-length, +start_thickness, +start_thickness);
    Vector p7(+length, +end_thickness, +end_thickness);

    // Add quads to new mesh
    Mesh m = Mesh("").shared_color(m_shared_color).texture_id(m_texture_id);
    m.add_quad(
      color, p0, p2, p3, p1, VectorNone(), fill_triangles, add_wireframe);
    m.add_quad(
      color, p1, p3, p7, p5, VectorNone(), fill_triangles, add_wireframe);
    m.add_quad(
      color, p4, p5, p7, p6, VectorNone(), fill_triangles, add_wireframe);
    m.add_quad(
      color, p4, p6, p2, p0, VectorNone(), fill_triangles, add_wireframe);
    m.add_quad(
      color, p2, p6, p7, p3, VectorNone(), fill_triangles, add_wireframe);
    m.add_quad(
      color, p4, p0, p1, p5, VectorNone(), fill_triangles, add_wireframe);

    // Transform if not unit thickline
    Transform transform = transform_init;
    if (start_point != VectorNone())
    {
      assert(end_point != VectorNone());
      Vector center = 0.5 * (start_point + end_point);
      Vector axis = end_point - start_point;
      length = axis.norm();
      Transform rotation = Transforms::rotation_to_align_x_to_axis(axis);
      Transform translation = Transforms::translate(center);
      Transform tx = translation * rotation;
      if (transform.isIdentity())
      {
        transform = tx;
      }
      else
      {
        transform *= tx;
      }
    }

    if (!transform.isIdentity())
    {
      m.apply_transform(transform);
    }

    this->append_mesh(m);
  }

  void Mesh::add_cone(
    const Color& color,
    const Transform& transform,
    float truncation_height,
    std::uint32_t lat_count,
    std::uint32_t long_count,
    bool fill_triangles,
    bool add_wireframe)
  {
    this->check_instances();
    this->check_color(color);

    const float radius = 0.5f;
    Vector apex(-0.5f, 0., 0.);
    bool add_apex = std::abs(truncation_height - 1.) < 1e-6;
    const float base_center_x = 0.5f;
    Mesh m = Mesh("").shared_color(m_shared_color);
    for (std::uint32_t lat_index = 0; lat_index < lat_count; ++lat_index)
    {
      float alpha = static_cast<float>(lat_index) / lat_count;
      alpha = alpha * truncation_height;

      for (std::uint32_t long_index = 0; long_index < long_count; ++long_index)
      {
        float phi = long_index * 2.0f * PI / long_count;
        float cosPhi = std::cos(phi);
        float sinPhi = std::sin(phi);

        Vector base_point(base_center_x, cosPhi * radius, sinPhi * radius);
        Vector xyz = interpolate(apex, base_point, alpha);
        Vector normal = (base_point - apex).cross(xyz - Vector(xyz(0), 0, 0));
        normal = normal.cross(base_point - apex);
        normal.normalize();
        m.append_vertex(xyz, normal, color);
      }
    }

    if (add_apex)
    {
      m.append_vertex(apex, Vector(-1, 0, 0), color);
    }

    for (std::uint32_t lat_index = 0; lat_index < lat_count - 1; ++lat_index)
    {
      for (std::uint32_t long_index = 0; long_index < long_count; ++long_index)
      {
        auto base_index = lat_index * long_count;
        auto a = base_index + long_index;
        auto b = base_index + (long_index + 1) % long_count;
        auto c = base_index + long_index + long_count;
        auto d = base_index + (long_index + 1) % long_count + long_count;
        if (fill_triangles)
        {
          m.append_triangle(b, a, c);
          m.append_triangle(b, c, d);
        }

        if (add_wireframe)
        {
          m.append_line(a, b);
          m.append_line(b, d);
          m.append_line(d, c);
          m.append_line(c, a);
        }
      }
    }

    if (add_apex)
    {
      std::uint32_t lat_index = lat_count - 1;
      auto a = lat_count * long_count;
      for (std::uint32_t long_index = 0; long_index < long_count; ++long_index)
      {
        auto base_index = lat_index * long_count;
        auto b = base_index + long_index;
        auto c = base_index + (long_index + 1) % long_count;

        if (fill_triangles)
        {
          m.append_triangle(b, a, c);
        }

        if (add_wireframe)
        {
          m.append_line(a, b);
          m.append_line(a, c);
        }
      }
    }

    if (!transform.isIdentity())
    {
      m.apply_transform(transform);
    }

    this->append_mesh(m);
  }

  void Mesh::add_coordinate_axes(
    float length, float thickness, const Transform& transform)
  {
    this->check_instances();
    if (this->m_vertices.cols() == 6)
    {
      // shared color mesh
      std::cerr << "Converting shared color mesh to use vertex color "
                << "to accommodate coordinate axes.";
      VertexBuffer vertices = VertexBuffer::Zero(this->count_vertices(), 9);
      if (this->count_vertices())
      {
        vertices.leftCols(6) = this->m_vertices;
        vertices.col(6).fill(this->m_shared_color.r());
        vertices.col(7).fill(this->m_shared_color.g());
        vertices.col(8).fill(this->m_shared_color.b());
        this->m_shared_color = Color::None();
      }

      this->m_vertices = vertices;
    }
    else if (this->m_vertices.cols() == 8)
    {
      // image/texture mesh
      throw std::invalid_argument("Cannot add coordinate axes to a UV mesh");
    }

    Mesh m = Mesh("");
    m.add_thickline(
      {1, 0, 0}, {0, 0, 0}, {length, 0, 0}, thickness, 0.5f * thickness);
    m.add_thickline(
      {0, 1, 0}, {0, 0, 0}, {0, length, 0}, thickness, 0.5f * thickness);
    m.add_thickline(
      {0, 0, 1}, {0, 0, 0}, {0, 0, length}, thickness, 0.5f * thickness);
    m.add_sphere({1, 1, 1}, Transforms::scale(thickness * 1.1f));
    if (!transform.isIdentity())
    {
      m.apply_transform(transform);
    }

    this->append_mesh(m);
  }

  void Mesh::add_camera_frustum(
    const Color& color,
    float fov_y_degrees,
    float aspect_ratio,
    float depth,
    float thickness,
    const Transform& transform)
  {
    this->check_instances();
    this->check_color(color);
    Mesh m = Mesh("").shared_color(m_shared_color);
    const float fov_y_half_radians = fov_y_degrees / 2.0f * PI / 180.0f;
    const float height = depth * std::sin(fov_y_half_radians);
    const float width = height * aspect_ratio;

    m.add_thickline(
      color, {0, 0, 0}, {+width, +height, depth}, 0.4f * thickness, thickness);
    m.add_thickline(
      color, {0, 0, 0}, {+width, -height, depth}, 0.4f * thickness, thickness);
    m.add_thickline(
      color, {0, 0, 0}, {-width, -height, depth}, 0.4f * thickness, thickness);
    m.add_thickline(
      color,
      {0.0, 0.0, 0.0},
      {-width, +height, depth},
      0.4f * thickness,
      thickness);
    m.add_thickline(
      color,
      {+width, +height, depth},
      {-width, +height, depth},
      thickness,
      thickness);
    m.add_thickline(
      color,
      {-width, +height, depth},
      {-width, -height, depth},
      thickness,
      thickness);
    m.add_thickline(
      color,
      {-width, -height, depth},
      {+width, -height, depth},
      thickness,
      thickness);
    m.add_thickline(
      color,
      {+width, -height, depth},
      {+width, +height, depth},
      thickness,
      thickness);
    m.add_coordinate_axes(depth * 0.075f, thickness);

    if (!transform.isIdentity())
    {
      m.apply_transform(transform);
    }

    this->append_mesh(m);
  }

  void Mesh::add_camera_frustum(
    const Camera& camera, const Color& color, float thickness, float depth)
  {
    this->check_instances();
    this->check_color(color);
    Mesh m = Mesh("").shared_color(m_shared_color);
    Vector eye(0, 0, 0);
    Eigen::Vector3f top_left(-1, -1, 1);
    Eigen::Vector3f top_right(1, -1, 1);
    Eigen::Vector3f bottom_left(-1, 1, 1);
    Eigen::Vector3f bottom_right(1, 1, 1);

    Transform unprojection = camera.projection().inverse();
    top_left =
      (unprojection * top_left.homogeneous()).hnormalized().normalized() *
      depth;
    top_right =
      (unprojection * top_right.homogeneous()).hnormalized().normalized() *
      depth;
    bottom_left =
      (unprojection * bottom_left.homogeneous()).hnormalized().normalized() *
      depth;
    bottom_right =
      (unprojection * bottom_right.homogeneous()).hnormalized().normalized() *
      depth;

    m.add_thickline(color, eye, bottom_right, 0.4f * thickness, thickness);
    m.add_thickline(color, eye, top_right, 0.4f * thickness, thickness);
    m.add_thickline(color, eye, top_left, 0.4f * thickness, thickness);
    m.add_thickline(color, eye, bottom_left, 0.4f * thickness, thickness);
    m.add_thickline(color, bottom_right, bottom_left, thickness, thickness);
    m.add_thickline(color, bottom_left, top_left, thickness, thickness);
    m.add_thickline(color, top_left, top_right, thickness, thickness);
    m.add_thickline(color, top_right, bottom_right, thickness, thickness);
    m.add_coordinate_axes(-0.075f, thickness);

    if (!camera.camera_to_world().isIdentity())
    {
      m.apply_transform(camera.camera_to_world());
    }

    this->append_mesh(m);
  }

  void Mesh::add_camera_image(const Camera& camera, float depth)
  {
    this->check_instances();
    Mesh m = Mesh("").shared_color(m_shared_color).texture_id(m_texture_id);
    Vector eye(0, 0, 0);
    Eigen::Vector3f top_left(-1, -1, 1);
    Eigen::Vector3f top_right(1, -1, 1);
    Eigen::Vector3f bottom_left(-1, 1, 1);
    Eigen::Vector3f bottom_right(1, 1, 1);

    Transform unprojection = camera.projection().inverse();
    top_left =
      (unprojection * top_left.homogeneous()).hnormalized().normalized() *
      depth;
    top_right =
      (unprojection * top_right.homogeneous()).hnormalized().normalized() *
      depth;
    bottom_left =
      (unprojection * bottom_left.homogeneous()).hnormalized().normalized() *
      depth;
    bottom_right =
      (unprojection * bottom_right.homogeneous()).hnormalized().normalized() *
      depth;

    m.add_quad(Color::None(), top_left, top_right, bottom_right, bottom_left);
    if (!camera.camera_to_world().isIdentity())
    {
      m.apply_transform(camera.camera_to_world());
    }

    this->append_mesh(m);
  }

  void Mesh::add_disc(
    const Color& color,
    const Transform& transform,
    std::uint32_t segment_count,
    bool fill_triangles,
    bool add_wireframe)
  {
    this->check_instances();
    this->check_color(color);

    float radius = 0.5f;

    VectorBuffer vertices =
      VectorBuffer::Zero(static_cast<Eigen::Index>(segment_count) + 1, 3);
    auto thetas = Eigen::ArrayXf::LinSpaced(segment_count, 0, 2.0f * PI);
    auto ys = radius * thetas.cos();
    auto zs = radius * thetas.sin();
    vertices.block(1, 1, segment_count, 1) = ys;
    vertices.bottomRightCorner(segment_count, 1) = zs;

    TriangleBuffer triangles(segment_count, 3);
    triangles.col(0).fill(0);
    triangles.col(2) = arange(0, segment_count) + 1;
    triangles.col(1) = roll(triangles.col(2).array(), 1);

    ColorBuffer colors = ColorBufferNone();
    if (!color.is_none())
    {
      colors = ColorBuffer(segment_count + 1, 3);
      colors.col(0).fill(color(0));
      colors.col(1).fill(color(1));
      colors.col(2).fill(color(2));
    }

    VectorBuffer normals =
      VectorBuffer::Zero(static_cast<Eigen::Index>(segment_count) + 1, 3);
    normals.col(0).fill(1.0f);

    this->add_mesh_with_normals(
      vertices,
      normals,
      triangles,
      colors,
      UVBufferNone(),
      transform,
      false,
      fill_triangles,
      add_wireframe);
  }

  void Mesh::add_cylinder(
    const Color& color,
    const Transform& transform,
    std::uint32_t segment_count,
    bool fill_triangles,
    bool add_wireframe)
  {
    this->check_instances();
    this->check_color(color);

    // Unit diameter for consistency with other primitives
    float radius = 0.5f;

    auto N = segment_count;

    // Add discs at each end of the cylinder
    // Note, we do not re-use disc vertices for the barrel, as this gives bad
    // normals
    std::array<float, 2> x_vals = {-0.5f, +0.5f};
    for (auto& x : x_vals)
    {
      auto disc_transform = Transforms::rotation_about_y(x == 0.5f ? 0 : PI);
      disc_transform = Transforms::translate({x, 0, 0}) * disc_transform;
      if (!transform.isIdentity())
      {
        disc_transform = transform * disc_transform;
      }

      this->add_disc(color, disc_transform, N, fill_triangles, add_wireframe);
    }

    auto thetas = Eigen::ArrayXf::LinSpaced(N, 0, 2.0f * PI);
    auto ys = radius * thetas.cos();
    auto zs = radius * thetas.sin();

    VectorBuffer vertices(2 * N, 3);
    vertices.topLeftCorner(N, 1).fill(-0.5f);
    vertices.bottomLeftCorner(N, 1).fill(+0.5f);
    vertices.block(0, 1, N, 1) = ys;
    vertices.block(N, 1, N, 1) = ys;
    vertices.topRightCorner(N, 1) = zs;
    vertices.bottomRightCorner(N, 1) = zs;

    VectorBuffer normals = vertices;
    normals.col(0).fill(0.0f);

    TriangleBuffer triangles(2 * N, 3);
    auto range = arange(0, N);
    triangles.topLeftCorner(N, 1) = roll(range, 1);
    triangles.block(0, 1, N, 1) = range;
    triangles.topRightCorner(N, 1) = range + N;
    triangles.bottomLeftCorner(N, 1) = roll(range, 1);
    triangles.block(N, 1, N, 1) = range + N;
    triangles.bottomRightCorner(N, 1) = roll(range, 1) + N;

    ColorBuffer colors = ColorBufferNone();
    if (!color.is_none())
    {
      colors = ColorBuffer(2 * N, 3);
      colors.col(0).fill(color(0));
      colors.col(1).fill(color(1));
      colors.col(2).fill(color(2));
    }

    this->add_mesh_with_normals(
      vertices,
      normals,
      triangles,
      colors,
      UVBufferNone(),
      transform,
      false,
      fill_triangles,
      add_wireframe);
  }

  void Mesh::add_sphere(
    const Color& color,
    const Transform& transform,
    bool fill_triangles,
    bool add_wireframe)
  {
    this->add_icosphere(color, transform, 2, fill_triangles, add_wireframe);
  }

  void Mesh::add_icosphere(
    const Color& color,
    const Transform& transform,
    std::uint32_t steps,
    bool fill_triangles,
    bool add_wireframe)
  {
    this->check_instances();
    this->check_color(color);

    // Create a basic icosohedron primitive

    // Leads to unit diameter for consistency with other primitives
    const float radius = 0.5f;
    const float t = 0.5f * (1.0f + std::sqrt(5.0f));
    VectorBuffer vertex_positions(12, 3);
    vertex_positions << -1.0, +t, 0.0, +1.0, +t, 0.0, -1.0, -t, 0.0, +1.0, -t,
      0.0, 0.0, -1.0, +t, 0.0, +1.0, +t, 0.0, -1.0, -t, 0.0, +1.0, -t, +t, 0.0,
      -1.0, +t, 0.0, +1.0, -t, 0.0, -1.0, -t, 0.0, +1.0;
    vertex_positions.rowwise().normalize();
    vertex_positions *= radius;

    TriangleBuffer triangles(20, 3);
    triangles << 0, 11, 5, 0, 5, 1, 0, 1, 7, 0, 7, 10, 0, 10, 11, 1, 5, 9, 5,
      11, 4, 11, 10, 2, 10, 7, 6, 7, 1, 8, 3, 9, 4, 3, 4, 2, 3, 2, 6, 3, 6, 8,
      3, 8, 9, 4, 9, 5, 2, 4, 11, 6, 2, 10, 8, 6, 7, 9, 8, 1;

    typedef std::pair<std::uint32_t, std::uint32_t> Edge;
    // Apply subdivision
    for (std::uint32_t iter = 0; iter < steps; ++iter)
    {
      std::map<Edge, std::uint32_t> new_e_vs;
      TriangleBuffer new_triangles;

      // For each triangle we will make up to 3 new e-verts
      // (some will have less when shared with neighbouring triangles)
      for (auto triangle = 0; triangle < triangles.rows(); ++triangle)
      {
        auto a = triangles(triangle, 0);
        auto b = triangles(triangle, 1);
        auto c = triangles(triangle, 2);
        std::map<Edge, std::uint32_t> v_idx;

        // Loop over triangle edges
        std::array<Edge, 3> edges = {Edge(a, b), Edge(a, c), Edge(b, c)};
        for (const auto& edge : edges)
        {
          Edge back_edge(edge.second, edge.first);
          // Check this edge does not already have a new edge-vertex
          if (new_e_vs.count(edge))
          {
            v_idx[edge] = new_e_vs[edge];
          }
          else if (new_e_vs.count(back_edge))
          {
            v_idx[edge] = new_e_vs[back_edge];
          }
          else
          {
            Vector v = vertex_positions.row(edge.first) +
              vertex_positions.row(edge.second);
            v.normalize();
            v *= radius;
            v_idx[edge] = new_e_vs[edge] =
              static_cast<std::uint32_t>(vertex_positions.rows());
            append_row(vertex_positions, v);
          }
        }

        // Append new triangles
        append_row(
          new_triangles, Triangle(a, v_idx[Edge(a, b)], v_idx[Edge(a, c)]));
        append_row(
          new_triangles,
          Triangle(v_idx[Edge(a, b)], v_idx[Edge(b, c)], v_idx[Edge(a, c)]));
        append_row(
          new_triangles, Triangle(v_idx[Edge(a, c)], v_idx[Edge(b, c)], c));
        append_row(
          new_triangles, Triangle(v_idx[Edge(a, b)], b, v_idx[Edge(b, c)]));
      }

      // Replace existing triangles with new triangles
      triangles = new_triangles;
    }

    // Set up per-vertex colors if required
    ColorBuffer colors;
    if (!color.is_none())
    {
      colors = ColorBuffer(vertex_positions.rows(), 3);
      colors.col(0).fill(color.r());
      colors.col(1).fill(color.g());
      colors.col(2).fill(color.b());
    }

    // Set up per-vertex uvs if required
    UVBuffer uvs;
    if (!m_texture_id.empty())
    {
      // Compute azimuth and inclination values into the uv buffer
      uvs.resize(vertex_positions.rows(), Eigen::NoChange);
      for (auto v_idx = 0; v_idx < vertex_positions.rows(); ++v_idx)
      {
        uvs(v_idx, 0) = static_cast<float>(
          0.5f -
          0.5f *
            std::atan2(vertex_positions(v_idx, 2), vertex_positions(v_idx, 0)) /
            PI); // Azimuth
        uvs(v_idx, 1) = static_cast<float>(
          1.0f -
          std::acos(vertex_positions(v_idx, 1) * 2.0f) / PI); // Inclination
      }

      // Duplicate vertices across the longitude seam - not particularly
      // efficient and bit of a hack but works ok.  Results in non watertight
      // mesh. Loop over triangles
      TriangleBuffer new_triangles;
      for (auto triangle = 0; triangle < triangles.rows(); ++triangle)
      {
        auto a = triangles(triangle, 0);
        auto b = triangles(triangle, 1);
        auto c = triangles(triangle, 2);

        // Rotate triangle indices to ensure a is east most vertex, preserving
        // triangle winding order
        for (std::size_t attempt = 0; attempt < 2; ++attempt)
        {
          if (uvs(a, 0) < uvs(b, 0) || uvs(a, 0) < uvs(c, 0))
          {
            std::tie(a, b, c) = std::make_tuple(b, c, a);
          }
        }

        // Identify hemisphere for a, b, c
        // Hack - doesn't look great at poles for small number of subdiv steps
        bool a_isEast = uvs(a, 0) > 0.66f;
        bool b_isWest = uvs(b, 0) < 0.33f;
        bool c_isWest = uvs(c, 0) < 0.33f;

        // Duplicate vertices if necessary
        if (a_isEast && c_isWest) // Duplicate c vertex
        {
          Vertex new_vertex = vertex_positions.row(c);
          append_row(vertex_positions, new_vertex); // Concatenate a copy of c
          append_row(uvs, Eigen::Vector2f(1.0f + uvs(c, 0), uvs(c, 1)));
          c = static_cast<uint32_t>(
            vertex_positions.rows() - 1); // Will overwrite c in triangle
        }
        if (a_isEast && b_isWest) // Duplicate b vertex
        {
          Vertex new_vertex = vertex_positions.row(b);
          append_row(vertex_positions, new_vertex); // Concatenate a copy of b
          append_row(uvs, Eigen::Vector2f(1.0f + uvs(b, 0), uvs(b, 1)));
          b = static_cast<uint32_t>(
            vertex_positions.rows() - 1); // Will overwrite b in triangle
        }

        // Save the new triangle
        append_row(new_triangles, Triangle(a, b, c));
      }

      // Replace existing triangles with new triangles
      triangles = new_triangles;
    }

    // Populate mesh

    // vertices double as normals given they lie on unit sphere
    this->add_mesh_with_normals(
      std::move(vertex_positions),
      std::move(vertex_positions),
      std::move(triangles),
      std::move(colors),
      std::move(uvs),
      transform,
      false,
      fill_triangles,
      add_wireframe);
  }

  void Mesh::add_uv_sphere(
    const Color& color,
    const Transform& transform,
    std::uint32_t lat_count,
    std::uint32_t long_count,
    bool fill_triangles,
    bool add_wireframe)
  {
    this->check_instances();
    this->check_color(color);

    // Add vertices

    // Leads to unit diameter for consistency with other primitives
    const double radius = 0.5;
    Mesh m = Mesh("").shared_color(m_shared_color);
    for (std::uint32_t lat_index = 0; lat_index < lat_count + 1; ++lat_index)
    {
      double theta = lat_index * M_PI / lat_count;
      double cosTheta = std::cos(theta);
      double sinTheta = std::sin(theta);
      for (std::uint32_t long_index = 0; long_index < long_count; ++long_index)
      {
        double phi = long_index * 2.0 * M_PI / long_count;
        double cosPhi = std::cos(phi);
        double sinPhi = std::sin(phi);

        double dx = radius * cosPhi * sinTheta;
        double dy = radius * cosTheta;
        double dz = radius * sinPhi * sinTheta;
        Vector pos = Eigen::Vector3d(dx, dy, dz).cast<float>();
        m.append_vertex(pos, pos, color);
      }
    }

    // Add triangles
    for (std::uint32_t lat_index = 0; lat_index < lat_count; ++lat_index)
    {
      for (std::uint32_t long_index = 0; long_index < long_count; ++long_index)
      {
        auto base_index = lat_index * long_count;
        auto a = base_index + long_index;
        auto b = base_index + (long_index + 1) % long_count;
        auto c = base_index + long_index + long_count;
        auto d = base_index + (long_index + 1) % long_count + long_count;
        if (fill_triangles)
        {
          if (lat_index > 0)
          {
            m.append_triangle(a, b, c);
          }
          m.append_triangle(c, b, d);
        }
        if (add_wireframe)
        {
          m.append_line(a, b);
          m.append_line(b, d);
          m.append_line(d, c);
          m.append_line(c, a);
        }
      }
    }

    if (!transform.isIdentity())
    {
      m.apply_transform(transform);
    }

    this->append_mesh(m);
  }

} // namespace scenepic