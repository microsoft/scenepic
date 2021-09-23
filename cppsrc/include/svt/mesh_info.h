// ----------------------------------------------------------------------------
//
// mesh_info.h -- Container for mesh creation information.
//
// Copyright (C) 2019 Microsoft
//
// For conditions of distribution and use, see copyright notice in LICENSE
//
// ----------------------------------------------------------------------------

#ifndef _SP_MESH_INFO_H_
#define _SP_MESH_INFO_H_

#include <memory>

#include "matrix.h"

namespace svt
{
/** Container for Mesh creation information */
class MeshInfo
{
public:
    /** Constructor. This will allocate buffers to the given sizes. Note: A mesh cannot
     *  have both UVs and color information, though it can have neither.
     * 
     *  \param num_vertices the number of vertices in the mesh
     *  \param num_triangles the number of triangles in the mesh
     *  \param has_uvs whether the mesh will have UV information
     *  \param has_colors whether the mesh will have per-vertex color information
     */
    MeshInfo(std::size_t num_vertices, std::size_t num_triangles, bool has_uvs, bool has_colors);

    /** A reference to the vector buffer containing the vertex positions. */
    VectorBufferRef position_buffer();

    /** A const reference to the vector buffer containing the vertex positions. */
    const ConstVectorBufferRef position_buffer() const;

    /** A reference to the triangle buffer containing the triangle vertex indices. */
    TriangleBufferRef triangle_buffer();

    /** A const reference to the triangle buffer containing the triangle vertex indices. */
    const ConstTriangleBufferRef triangle_buffer() const;

    /** A reference to the UV buffer containing the per-vertex UV values. */
    UVBufferRef uv_buffer();

    /** A const reference to the UV buffer containing the per-vertex UV values. */
    const ConstUVBufferRef uv_buffer() const;

    /** A reference to the color buffer containing the per-vertex color values. */
    ColorBufferRef color_buffer();

    /** A const reference to the color buffer containing the per-vertex color values. */
    const ConstColorBufferRef color_buffer() const;

    /** Subdivide this mesh using loop subdivision.
     * 
     *  \param steps specifies how many steps of subdivision to apply. Defaults to 1.
     *  \param project_to_limit specifies whether the vertices should be projected onto the
     *                          limit surface in the final step of subdivision. Defaults to false.
     *  \return a subdivided version of this mesh
     */ 
    std::shared_ptr<MeshInfo> subdivide(int steps = 1, bool project_to_limit = false);


private:
    VectorBuffer m_position_buffer;
    TriangleBuffer m_triangle_buffer;
    UVBuffer m_uv_buffer;
    ColorBuffer m_color_buffer;
};
} // namespace svt

#endif