// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#ifndef _SCENEPIC_LOOP_SUBDIV_STENCIL_H_
#define _SCENEPIC_LOOP_SUBDIV_STENCIL_H_

#include "matrix.h"
#include "mesh_info.h"

namespace scenepic
{
  /** This class enables efficient Loop subdivision of triangle meshes.
   *
   *  The subdivision stencil specifies what linear combinations of existing
   *  vertices are needed, and stencils can be reused across meshes with
   * different vertex positions but identical triangle topologies. Thus, the
   * constructor only takes the triangles, and caches the stencil. The stencil
   * can be applied to a set of vertices using the apply() method.
   */
  class LoopSubdivisionStencil
  {
  public:
    /** Constructor.
     *  \param triangles the triangles of the subdivided mesh
     *  \param subdiv the subdivision matrix of size (num_new_verts,
     *                num_old_verts) that maps old vertices to new ones
     */
    LoopSubdivisionStencil(TriangleBuffer triangles, SparseMatrix subdiv);

    /** Returns the triangles of the subdivided mesh. */
    const ConstTriangleBufferRef triangles() const;

    /** Applies the stencil to a vertex buffer.
     *
     *  \param vertices the vertices to map.
     *  \return new vertices constructed from the old ones using the stencil
     */
    VertexBuffer apply(const ConstVertexBufferRef vertices) const;

    /** The number of vertices in the subdivided mesh. */
    std::size_t vertex_count() const;

    /** The number of triangles in the subdivided mesh. */
    std::size_t triangle_count() const;

    /** Creates a new stencil by subdividing the provided triangles.
     *
     *  \param triangles the initial triangle indices
     *  \param steps specifies how many steps of subdivision to apply.
     *               Defaults to 1.
     *  \param project_to_limit specifies whether the vertices should be
     *                          projected onto the limit surface in the final
     *                          step of subdivision. Defaults to false.
     *  \return a stencil object
     */
    static LoopSubdivisionStencil create(
      const ConstTriangleBufferRef triangles,
      int steps = 1,
      bool project_to_limit = false);

  private:
    TriangleBuffer m_triangles;
    SparseMatrix m_subdiv;
  };
} // namespace scenepic

#endif