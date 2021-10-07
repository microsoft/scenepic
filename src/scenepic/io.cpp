// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include <algorithm>
#include <array>
#include <cstdint>
#include <exception>
#include <fstream>
#include <map>
#include <sstream>
#include <tuple>
#include <vector>

#define TINYOBJLOADER_IMPLEMENTATION
#include "io.h"
#include "js_lib.h"
#include "mesh_info.h"
#include "tinyobj/tiny_obj_loader.h"
#include "util.h"

namespace
{
  typedef std::tuple<std::int32_t, std::int32_t, std::int32_t> Corner;
  typedef std::tuple<float, float, float> Vec3;
  typedef std::tuple<float, float> Vec2;

  struct MeshData
  {
    std::vector<Vec3> positions;
    std::vector<Vec3> normals;
    std::vector<Vec2> uvs;
    std::vector<Corner> corners;
  };

  Corner resolve_corner(
    const Corner& vertex,
    const std::vector<std::size_t>& position_index,
    const std::vector<std::size_t>& uv_index,
    const std::vector<std::size_t>& normal_index)
  {
    std::int32_t pos_idx, uv_idx, norm_idx;
    std::tie(pos_idx, uv_idx, norm_idx) = vertex;

    if (pos_idx < 0)
    {
      pos_idx = static_cast<std::int32_t>(
        position_index[position_index.size() + pos_idx]);
    }
    else if (pos_idx > 0)
    {
      pos_idx = static_cast<std::int32_t>(position_index[pos_idx - 1]);
    }

    if (uv_idx < 0)
    {
      uv_idx = static_cast<std::int32_t>(uv_index[uv_index.size() + uv_idx]);
    }
    else if (uv_idx > 0)
    {
      uv_idx = static_cast<std::int32_t>(uv_index[uv_idx - 1]);
    }
    else
    {
      uv_idx = -1;
    }

    if (norm_idx < 0)
    {
      norm_idx =
        static_cast<std::int32_t>(normal_index[normal_index.size() + norm_idx]);
    }
    else if (norm_idx > 0)
    {
      norm_idx = static_cast<std::int32_t>(normal_index[norm_idx - 1]);
    }
    else
    {
      norm_idx = -1;
    }

    return std::make_tuple(pos_idx, uv_idx, norm_idx);
  }

} // namespace

namespace scenepic
{
  std::shared_ptr<MeshInfo> load_obj(const std::string& path)
  {
    std::ifstream stream(path);
    if (stream.is_open())
      return load_obj(stream);

    throw std::invalid_argument("Unable to open file.");
  }

  std::shared_ptr<MeshInfo> load_obj(std::istream& stream)
  {
    MeshData mesh_data;

    std::string warn;
    std::string err;

    tinyobj::callback_t cb;
    cb.vertex_cb = [](void* user_data, float x, float y, float z, float w) {
      auto mesh_data = reinterpret_cast<MeshData*>(user_data);
      mesh_data->positions.emplace_back(x / w, y / w, z / w);
    };
    cb.texcoord_cb = [](void* user_data, float x, float y, float z) {
      auto mesh_data = reinterpret_cast<MeshData*>(user_data);
      mesh_data->uvs.emplace_back(x, y);
    };
    cb.normal_cb = [](void* user_data, float x, float y, float z) {
      auto mesh_data = reinterpret_cast<MeshData*>(user_data);
      mesh_data->normals.emplace_back(x, y, z);
    };
    cb.index_cb =
      [](void* user_data, tinyobj::index_t* indices, int num_indices) {
        auto mesh_data = reinterpret_cast<MeshData*>(user_data);
        if (num_indices == 3)
        {
          for (auto idx = indices; idx < indices + 3; ++idx)
          {
            mesh_data->corners.emplace_back(
              idx->vertex_index, idx->texcoord_index, idx->normal_index);
          }
        }
        else if (num_indices == 4)
        {
          for (auto idx = indices; idx < indices + 3; ++idx)
          {
            mesh_data->corners.emplace_back(
              idx->vertex_index, idx->texcoord_index, idx->normal_index);
          }

          for (auto idx = indices + 2; idx < indices + 4; ++idx)
          {
            mesh_data->corners.emplace_back(
              idx->vertex_index, idx->texcoord_index, idx->normal_index);
          }

          mesh_data->corners.emplace_back(
            indices->vertex_index,
            indices->texcoord_index,
            indices->normal_index);
        }
        else
        {
          throw std::invalid_argument(
            "Only triangles and quad meshes are supported.");
        }
      };

    bool ret = tinyobj::LoadObjWithCallback(stream, cb, &mesh_data, NULL, &err);

    std::vector<Vec3> unique_positions;
    std::vector<std::size_t> position_index;
    unique_index(mesh_data.positions, unique_positions, position_index);

    std::vector<Vec2> unique_uvs;
    std::vector<std::size_t> uv_index;
    unique_index(mesh_data.uvs, unique_uvs, uv_index);

    std::vector<Vec3> unique_normals;
    std::vector<std::size_t> normal_index;
    unique_index(mesh_data.normals, unique_normals, normal_index);

    std::transform(
      mesh_data.corners.begin(),
      mesh_data.corners.end(),
      mesh_data.corners.begin(),
      [&](const Corner& vertex) -> Corner {
        return resolve_corner(vertex, position_index, uv_index, normal_index);
      });

    std::vector<Corner> unique_corners;
    std::vector<std::size_t> corner_index;
    unique_index(mesh_data.corners, unique_corners, corner_index);

    auto num_vertices = unique_corners.size();
    auto num_triangles = corner_index.size() / 3;
    bool has_uvs = mesh_data.uvs.size() != 0;
    bool has_normals = mesh_data.normals.size() != 0;

    auto mesh_info = std::make_shared<MeshInfo>(
      num_vertices, num_triangles, has_uvs, has_normals, false);

    for (auto i = 0; i < num_vertices; ++i)
    {
      std::int32_t pos_idx, uv_idx, norm_idx;
      std::tie(pos_idx, uv_idx, norm_idx) = unique_corners[i];
      float x, y, z;
      std::tie(x, y, z) = unique_positions[pos_idx];
      mesh_info->position_buffer().row(i) << x, y, z;
      if (uv_idx >= 0)
      {
        float u, v;
        std::tie(u, v) = unique_uvs[uv_idx];
        mesh_info->uv_buffer().row(i) << u, v;
      }

      if (norm_idx >= 0)
      {
        float x, y, z;
        std::tie(x, y, z) = unique_normals[norm_idx];
        mesh_info->normal_buffer().row(i) << x, y, z;
      }
    }

    for (auto i = 0, j = 0; i < num_triangles; ++i, j += 3)
    {
      std::uint32_t a = static_cast<std::uint32_t>(corner_index[j]);
      std::uint32_t b = static_cast<std::uint32_t>(corner_index[j + 1]);
      std::uint32_t c = static_cast<std::uint32_t>(corner_index[j + 2]);
      mesh_info->triangle_buffer().row(i) << a, b, c;
    }

    return mesh_info;
  }
} // namespace scenepic