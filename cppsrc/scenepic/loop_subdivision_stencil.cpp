// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#define _USE_MATH_DEFINES
#include "loop_subdivision_stencil.h"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <map>
#include <utility>
#include <vector>

namespace scenepic
{
  typedef std::map<Triangle::Scalar, Triangle::Scalar> VertexMap;
  typedef std::vector<Triangle::Scalar> VertexList;
  typedef std::map<Triangle::Scalar, VertexList> VertexListMap;
  typedef std::vector<VertexMap> VertexMapList;
  typedef std::pair<Triangle::Scalar, Triangle::Scalar> Edge;
  typedef std::map<Edge, Triangle::Scalar> EdgeVertexMap;
  typedef std::vector<Edge> EdgeList;

  const std::size_t MIN_VALENCY = 3;
  const std::size_t MAX_VALENCY = 10;

  Edge reverse(const Edge& edge)
  {
    return std::make_pair(edge.second, edge.first);
  }

  VertexList keys(const VertexMap& vertex_map)
  {
    VertexList result;
    result.reserve(vertex_map.size());
    std::transform(
      vertex_map.begin(),
      vertex_map.end(),
      std::back_inserter(result),
      [](const VertexMap::value_type& pair) { return pair.first; });
    return result;
  }

  struct SubdivisionBuilder
  {
    SubdivisionBuilder() : alphas(MAX_VALENCY + 1), omegas(MAX_VALENCY + 1)
    {
      // Compute alphas and omegas
      std::vector<double> alphas(MAX_VALENCY + 1);
      std::vector<double> omegas(MAX_VALENCY + 1);
      for (auto N = MIN_VALENCY; N < MAX_VALENCY + 1; ++N)
      {
        auto val = 3.0 + 2.0 * std::cos(2.0 * M_PI / N);
        auto val2 = val * val;
        this->alphas[N] = 5.0 / 8.0 - val2 / 64.0;
        this->omegas[N] = 24.0 * N / (40.0 - val2);
      }
    }

    Triangle::Scalar
    add_new_vertex(const VertexList& idxs, const std::vector<float>& weights)
    {
      auto new_index = this->v_indptr.size();
      this->v_indptr.push_back(
        static_cast<Triangle::Scalar>(this->v_weights.size()));
      this->v_weights.insert(
        this->v_weights.end(), weights.begin(), weights.end());
      this->v_generating_idxs.insert(
        this->v_generating_idxs.end(), idxs.begin(), idxs.end());
      return static_cast<Triangle::Scalar>(new_index);
    }

    void generate_vertex_vertex(
      Triangle::Scalar i_vertex, const VertexListMap& v_neighbors, int steps)
    {
      // construct stencils for original vertices
      auto& ns = v_neighbors.at(i_vertex);
      auto valency = ns.size();
      std::vector<float> weights(valency + 1);
      if (steps == 0)
      {
        auto omega = this->omegas[valency];
        weights[0] = static_cast<float>(omega / (omega + valency));
        std::fill(
          weights.begin() + 1,
          weights.end(),
          static_cast<float>(1.0 / (omega + valency)));
      }
      else
      {
        auto alpha = this->alphas[valency];
        weights[0] = static_cast<float>(1 - alpha);
        std::fill(
          weights.begin() + 1,
          weights.end(),
          static_cast<float>(alpha / valency));
      }

      VertexList idxs = {i_vertex};
      idxs.insert(idxs.end(), ns.begin(), ns.end());
      this->v_idx[i_vertex] = this->add_new_vertex(idxs, weights);
    }

    void
    add_triangle(Triangle::Scalar a, Triangle::Scalar b, Triangle::Scalar c)
    {
      this->triangles.emplace_back(
        this->v_idx[a], this->v_idx[b], this->v_idx[c]);
    }

    void add_triangles(
      const VertexListMap& v_neighbors,
      Triangle::Scalar a,
      Triangle::Scalar b,
      Triangle::Scalar c)
    {
      Edge ab = std::make_pair(a, b);
      Edge bc = std::make_pair(b, c);
      Edge ca = std::make_pair(c, a);
      if (v_neighbors.count(a))
      {
        this->triangles.emplace_back(
          this->v_idx[a], this->ev_idx[ab], this->ev_idx[ca]);
      }

      if (this->has_vertex(ab) && this->has_vertex(bc) && this->has_vertex(ca))
      {
        this->triangles.emplace_back(
          this->ev_idx[ab], this->ev_idx[bc], this->ev_idx[ca]);
      }

      if (v_neighbors.count(c))
      {
        this->triangles.emplace_back(
          this->ev_idx[ca], this->ev_idx[bc], this->v_idx[c]);
      }

      if (v_neighbors.count(b))
      {
        this->triangles.emplace_back(
          this->ev_idx[ab], this->v_idx[b], this->ev_idx[bc]);
      }
    }

    void
    generate_edge_vertex(const Edge& edge, const VertexMapList& v_1ringlinks)
    {
      auto n1 = v_1ringlinks[edge.first].at(edge.second);
      auto n2 = v_1ringlinks[edge.second].at(edge.first);
      VertexList idxs = {edge.first, edge.second, n1, n2};
      std::vector<float> weights = {0.375f, 0.375f, 0.125f, 0.125f};
      this->ev_idx[edge] = this->add_new_vertex(idxs, weights);
      this->ev_idx[reverse(edge)] = this->ev_idx[edge];
    }

    bool has_vertex(Triangle::Scalar i)
    {
      return this->v_idx.count(i);
    }

    bool has_vertex(const Edge& edge)
    {
      return this->ev_idx.count(edge);
    }

    LoopSubdivisionStencil build(std::size_t vertex_count)
    {
      TriangleBuffer triangles(this->triangles.size(), 3);
      for (auto row = 0; row < triangles.rows(); ++row)
      {
        triangles.row(row) = this->triangles[row];
      }

      auto rows = this->v_indptr.size();
      auto cols = vertex_count;
      typedef Eigen::Triplet<float> T;
      std::vector<T> triplets;
      Triangle::Scalar num_nonzero =
        static_cast<Triangle::Scalar>(this->v_weights.size());
      triplets.reserve(num_nonzero);
      this->v_indptr.push_back(num_nonzero);
      for (auto row = 0; row < rows; ++row)
      {
        auto start = this->v_indptr[row];
        auto end = this->v_indptr[row + 1];
        for (auto j = start; j < end; ++j)
        {
          auto col = this->v_generating_idxs[j];
          triplets.emplace_back(row, col, this->v_weights[j]);
        }
      }
      SparseMatrix subdiv(rows, cols);
      subdiv.setFromTriplets(triplets.begin(), triplets.end());
      return LoopSubdivisionStencil(triangles, subdiv);
    }

    std::vector<double> alphas;
    std::vector<double> omegas;
    VertexMap v_idx;
    EdgeVertexMap ev_idx;
    std::vector<Triangle> triangles;
    std::vector<float> v_weights;
    VertexList v_generating_idxs;
    VertexList v_indptr;
  };

  EdgeList edges(Triangle::Scalar a, Triangle::Scalar b, Triangle::Scalar c)
  {
    return {std::make_pair(a, b), std::make_pair(b, c), std::make_pair(c, a)};
  }

  const ConstTriangleBufferRef LoopSubdivisionStencil::triangles() const
  {
    return m_triangles;
  }

  std::size_t LoopSubdivisionStencil::vertex_count() const
  {
    return static_cast<std::size_t>(m_subdiv.rows());
  }

  std::size_t LoopSubdivisionStencil::triangle_count() const
  {
    return static_cast<std::size_t>(m_triangles.rows());
  }

  VertexBuffer
  LoopSubdivisionStencil::apply(const ConstVertexBufferRef vertices) const
  {
    return m_subdiv * vertices;
  }

  LoopSubdivisionStencil::LoopSubdivisionStencil(
    TriangleBuffer triangles, SparseMatrix subdiv)
  : m_triangles(triangles), m_subdiv(subdiv)
  {}

  LoopSubdivisionStencil LoopSubdivisionStencil::create(
    const ConstTriangleBufferRef triangles, int steps, bool project_to_limit)
  {
    assert(steps > 0 || (steps == 0 && project_to_limit));

    // Build an index that gives the opposite vertex in a triangle, given a
    // vertex and an outward pointing half-edge (encoded here using the index of
    // the vertex at the end of the edge)
    auto vertex_count = triangles.maxCoeff() + 1;
    VertexMapList v_1ringlinks(vertex_count);
    for (auto row = 0; row < triangles.rows(); ++row)
    {
      auto tri_verts = triangles.row(row);
      for (auto i = 0; i < 3; ++i)
      {
        v_1ringlinks[tri_verts[i]][tri_verts[(i + 1) % 3]] =
          tri_verts[(i + 2) % 3];
      }
    }

    // Now get vertex neighbours ordered by connectivity around each vertex
    // The resulting dict contains only internal (non-boundary) vertices.
    // Each entry in the returned dictionary maps from the index of the
    // internal vertex to a list of neighbouring vertex indices
    VertexListMap v_neighbors;
    for (auto it = v_1ringlinks.begin(); it < v_1ringlinks.end(); ++it)
    {
      auto i_vert = static_cast<VertexMap::key_type>(it - v_1ringlinks.begin());
      auto ringlinks = *it;
      // Pick the first neighbour arbitrarily
      VertexList onering = {keys(ringlinks)[0]};
      // Keep appending neighbors until we complete the 1-ring
      while (ringlinks[onering.back()] != onering.front())
      {
        onering.push_back(ringlinks[onering.back()]);
        if (!ringlinks.count(onering.back()))
        {
          // Boundary vertex: incomplete 1-ring
          onering.clear();
          break;
        }
      }

      if (onering.size())
      {
        v_neighbors[i_vert] = onering;
      }
    }

    SubdivisionBuilder builder;
    // For each triangle we will make up to 3 new v-verts and 3 new e-verts
    for (auto row = 0; row < triangles.rows(); ++row)
    {
      auto tri = triangles.row(row);
      // Loop over each vertex in the triangle to make vertex-vertices
      for (auto col = 0; col < 3; ++col)
      {
        auto i = tri(col);
        // Check if this vertex already has a new vertex or is on the boundary
        if (builder.has_vertex(i) || v_neighbors.count(i) == 0)
        {
          continue;
        }

        builder.generate_vertex_vertex(i, v_neighbors, steps);
      }

      auto a = tri(0);
      auto b = tri(1);
      auto c = tri(2);
      if (steps == 0)
      {
        if (
          v_neighbors.count(a) && v_neighbors.count(b) && v_neighbors.count(c))
        {
          builder.add_triangle(a, b, c);
        }
        // No edge vertices for project_to_limit
        continue;
      }

      // Loop over each edge in the triangle to make edge-vertices
      for (auto& edge : edges(a, b, c))
      {
        // Check if this edge already has a new edge-vertex
        if (builder.has_vertex(edge))
        {
          continue;
        }
        // Or if it's a boundary edge
        if (
          v_neighbors.count(edge.first) == 0 &&
          v_neighbors.count(edge.second) == 0)
        {
          continue;
        }

        builder.generate_edge_vertex(edge, v_1ringlinks);
      }

      builder.add_triangles(v_neighbors, a, b, c);
    }

    auto stencil = builder.build(vertex_count);
    if (steps > 1 || (steps == 1 && project_to_limit))
    {
      auto next_stencil = LoopSubdivisionStencil::create(
        stencil.m_triangles, steps - 1, project_to_limit);
      return LoopSubdivisionStencil(
        next_stencil.m_triangles, next_stencil.m_subdiv * stencil.m_subdiv);
    }

    return stencil;
  }
} // namespace scenepic