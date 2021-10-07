// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "mesh_info.h"

#include "loop_subdivision_stencil.h"

#include <exception>

namespace scenepic
{
  MeshInfo::MeshInfo(
    std::size_t num_vertices,
    std::size_t num_triangles,
    bool has_uvs,
    bool has_normals,
    bool has_colors)
  {
    if (has_uvs && has_colors)
    {
      throw std::invalid_argument(
        "A mesh cannot have both UV information and per-vertex color.");
    }

    m_position_buffer = VectorBuffer(num_vertices, 3);
    m_triangle_buffer = TriangleBuffer(num_triangles, 3);

    if (has_uvs)
    {
      m_uv_buffer = UVBuffer(num_vertices, 2);
    }
    else
    {
      m_uv_buffer = UVBufferNone();
    }

    if (has_normals)
    {
      m_normal_buffer = VectorBuffer(num_vertices, 3);
    }
    else
    {
      m_normal_buffer = VectorBufferNone();
    }

    if (has_colors)
    {
      m_color_buffer = ColorBuffer(num_vertices, 3);
    }
    else
    {
      m_color_buffer = ColorBufferNone();
    }
  }

  VectorBufferRef MeshInfo::position_buffer()
  {
    return VectorBufferRef(m_position_buffer);
  }

  const ConstVectorBufferRef MeshInfo::position_buffer() const
  {
    return ConstVectorBufferRef(m_position_buffer);
  }

  TriangleBufferRef MeshInfo::triangle_buffer()
  {
    return TriangleBufferRef(m_triangle_buffer);
  }

  const ConstTriangleBufferRef MeshInfo::triangle_buffer() const
  {
    return ConstTriangleBufferRef(m_triangle_buffer);
  }

  UVBufferRef MeshInfo::uv_buffer()
  {
    return UVBufferRef(m_uv_buffer);
  }

  const ConstUVBufferRef MeshInfo::uv_buffer() const
  {
    return ConstUVBufferRef(m_uv_buffer);
  }

  VectorBufferRef MeshInfo::normal_buffer()
  {
    return VectorBufferRef(m_normal_buffer);
  }

  const ConstVectorBufferRef MeshInfo::normal_buffer() const
  {
    return ConstVectorBufferRef(m_normal_buffer);
  }

  bool MeshInfo::has_normals() const
  {
    return m_normal_buffer.size();
  }

  ColorBufferRef MeshInfo::color_buffer()
  {
    return ColorBufferRef(m_color_buffer);
  }

  const ConstColorBufferRef MeshInfo::color_buffer() const
  {
    return ConstColorBufferRef(m_color_buffer);
  }

  std::shared_ptr<MeshInfo>
  MeshInfo::subdivide(int steps, bool project_to_limit)
  {
    auto stencil = LoopSubdivisionStencil::create(
      m_triangle_buffer, steps, project_to_limit);
    bool has_uvs = m_uv_buffer.size();
    bool has_color = m_color_buffer.size();
    bool has_normals = m_normal_buffer.size();
    auto subdiv_mesh = std::make_shared<MeshInfo>(
      stencil.vertex_count(),
      stencil.triangle_count(),
      has_uvs,
      has_normals,
      has_color);
    subdiv_mesh->m_position_buffer = stencil.apply(m_position_buffer);
    subdiv_mesh->m_triangle_buffer = stencil.triangles();
    if (has_color)
    {
      subdiv_mesh->m_color_buffer = stencil.apply(m_color_buffer);
    }

    if (has_normals)
    {
      subdiv_mesh->m_normal_buffer = stencil.apply(m_normal_buffer);
    }

    if (has_uvs)
    {
      subdiv_mesh->m_uv_buffer = stencil.apply(m_uv_buffer);
    }

    return subdiv_mesh;
  }

} // namespace scenepic