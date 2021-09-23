#include <exception>

#include "loop_subdivision_stencil.h"
#include "mesh_info.h"

namespace svt
{
MeshInfo::MeshInfo(std::size_t num_vertices, std::size_t num_triangles, bool has_uvs, bool has_colors)
{
    if (has_uvs && has_colors)
    {
        throw std::invalid_argument("A mesh cannot have both UV information and per-vertex color.");
    }

    this->m_position_buffer = VectorBuffer(num_vertices, 3);
    this->m_triangle_buffer = TriangleBuffer(num_triangles, 3);

    if (has_uvs)
    {
        this->m_uv_buffer = UVBuffer(num_vertices, 2);
    }
    else
    {
        this->m_uv_buffer = UVBufferNone();
    }

    if (has_colors)
    {
        this->m_color_buffer = ColorBuffer(num_vertices, 3);
    }
    else
    {
        this->m_color_buffer = ColorBufferNone();
    }
}

VectorBufferRef MeshInfo::position_buffer()
{
    return VectorBufferRef(this->m_position_buffer);
}

const ConstVectorBufferRef MeshInfo::position_buffer() const
{
    return ConstVectorBufferRef(this->m_position_buffer);
}

TriangleBufferRef MeshInfo::triangle_buffer()
{
    return TriangleBufferRef(this->m_triangle_buffer);
}

const ConstTriangleBufferRef MeshInfo::triangle_buffer() const
{
    return ConstTriangleBufferRef(this->m_triangle_buffer);
}

UVBufferRef MeshInfo::uv_buffer()
{
    return UVBufferRef(this->m_uv_buffer);
}

const ConstUVBufferRef MeshInfo::uv_buffer() const
{
    return ConstUVBufferRef(this->m_uv_buffer);
}

ColorBufferRef MeshInfo::color_buffer()
{
    return ColorBufferRef(this->m_color_buffer);
}

const ConstColorBufferRef MeshInfo::color_buffer() const
{
    return ConstColorBufferRef(this->m_color_buffer);
}

std::shared_ptr<MeshInfo> MeshInfo::subdivide(int steps, bool project_to_limit)
{
    auto stencil = LoopSubdivisionStencil::create(this->m_triangle_buffer, steps, project_to_limit);
    bool has_uvs = this->m_uv_buffer.size();
    bool has_color = this->m_color_buffer.size();
    auto subdiv_mesh = std::make_shared<MeshInfo>(stencil.vertex_count(), stencil.triangle_count(), has_uvs, has_color);
    subdiv_mesh->m_position_buffer = stencil.apply(this->m_position_buffer);
    subdiv_mesh->m_triangle_buffer = stencil.triangles();
    if(has_color)
    {
        subdiv_mesh->m_color_buffer = stencil.apply(this->m_color_buffer);
    }

    if(has_uvs)
    {
        subdiv_mesh->m_uv_buffer = stencil.apply(this->m_uv_buffer);
    }

    return subdiv_mesh;
}

} // namespace svt