// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "scene.h"

#include <algorithm>
#include <exception>
#include <sstream>
#include <tuple>

namespace
{
  const float NUM_BINS =
    static_cast<float>(scenepic::MeshUpdate::QuantizationBinCount);

  float compute_representable_range(
    float relative_error_threshold, float absolute_error_threshold, float range)
  {
    float absolute_range = NUM_BINS * absolute_error_threshold;
    float relative_range = NUM_BINS * (relative_error_threshold * range);
    if (relative_error_threshold > 0)
    {
      if (absolute_error_threshold > 0)
      {
        return std::min(absolute_range, relative_range);
      }
      else
      {
        return relative_range;
      }
    }
    else
    {
      if (absolute_error_threshold > 0)
      {
        return absolute_range;
      }
      else
      {
        throw std::invalid_argument(
          "One of the thresholds must be greater than 0.");
      }
    }
  }

  float estimate_size_ratio(
    std::size_t num_keyframes, std::size_t num_updates, bool per_frame_range)
  {
    float num_deltas = static_cast<float>(num_updates - num_keyframes);
    float keyframe_size = static_cast<float>(num_keyframes) * 4.0f;
    float delta_size = static_cast<float>(num_deltas) * 2.0f;
    float uncompressed_size = static_cast<float>(num_updates) * 4.0f;
    if (!per_frame_range)
    {
      // sharing the quantization range tends to result in a further
      // reduction in size. This value is an empirically-determined heuristic.
      delta_size *= 0.75f;
    }

    return (keyframe_size + delta_size) / uncompressed_size;
  }
} // namespace

namespace scenepic
{
  QuantizationInfo::QuantizationInfo(
    std::uint32_t keyframe_count,
    float estimated_size_ratio,
    float mean_error,
    float max_error)
  : keyframe_count(keyframe_count),
    estimated_size_ratio(estimated_size_ratio),
    mean_error(mean_error),
    max_error(max_error)
  {}

  std::string QuantizationInfo::to_string() const
  {
    std::stringstream result;
    result << "QuantizationInfo("
           << "keyframe_count=" << this->keyframe_count << ", "
           << "estimated_size_ratio=" << this->estimated_size_ratio << ", "
           << "mean_error=" << this->mean_error << ", "
           << "max_error=" << this->max_error << ")";

    return result.str();
  }

  struct KeyframeAssignment
  {
    KeyframeAssignment(
      const std::shared_ptr<MeshUpdate>& frame,
      const std::shared_ptr<MeshUpdate>& keyframe)
    : frame_index(frame->frame_index()),
      keyframe_index(keyframe->frame_index()),
      range(frame->difference_range(keyframe->vertex_buffer()))
    {}

    bool is_keyframe() const
    {
      return this->frame_index == this->keyframe_index;
    }

    bool operator<(const KeyframeAssignment& other) const
    {
      return this->range < other.range;
    }

    std::uint32_t frame_index;
    std::uint32_t keyframe_index;
    float range;
  };

  std::vector<KeyframeAssignment> update_assignments(
    const std::vector<KeyframeAssignment> assignments,
    const std::vector<std::shared_ptr<MeshUpdate>>& updates,
    const std::shared_ptr<MeshUpdate>& keyframe)
  {
    std::vector<KeyframeAssignment> new_assignments;
    for (auto assignment : assignments)
    {
      KeyframeAssignment new_assignment(
        updates[assignment.frame_index], keyframe);
      if (new_assignment.range < assignment.range)
      {
        new_assignments.push_back(new_assignment);
      }
      else
      {
        new_assignments.push_back(assignment);
      }

      std::push_heap(new_assignments.begin(), new_assignments.end());
    }

    return new_assignments;
  }

  QuantizationInfo quantize_updates_for_mesh(
    float representable_range,
    std::vector<std::shared_ptr<MeshUpdate>>& updates,
    bool per_frame_range)
  {
    std::vector<KeyframeAssignment> assignments;
    auto keyframe = updates[0];
    for (auto& frame : updates)
    {
      assignments.emplace_back(frame, keyframe);
      std::push_heap(assignments.begin(), assignments.end());
    }

    while (assignments[0].range > representable_range)
    {
      keyframe = updates[assignments[0].frame_index];
      assignments = update_assignments(assignments, updates, keyframe);
    }

    float error_sum = 0;
    float max_error = 0;
    std::uint32_t num_keyframes = 0;
    for (auto& assignment : assignments)
    {
      if (assignment.is_keyframe())
      {
        ++num_keyframes;
      }
      else
      {
        float frame_range = representable_range;
        auto keyframe_vertex_buffer =
          updates[assignment.keyframe_index]->vertex_buffer();
        if (per_frame_range)
        {
          frame_range = updates[assignment.frame_index]->difference_range(
            keyframe_vertex_buffer);
        }

        float error = frame_range / NUM_BINS;
        max_error = std::max(max_error, error);
        error_sum += error;

        updates[assignment.frame_index]->quantize(
          assignment.keyframe_index, frame_range, keyframe_vertex_buffer);
      }
    }

    float mean_error =
      error_sum / static_cast<float>(updates.size() - num_keyframes);
    float estimated_size_ratio =
      estimate_size_ratio(num_keyframes, updates.size(), per_frame_range);

    return QuantizationInfo(
      num_keyframes, estimated_size_ratio, mean_error, max_error);
  }

  float Scene::compute_mesh_range(const std::string& mesh_id)
  {
    auto it = std::find_if(
      m_meshes.begin(), m_meshes.end(), [&](const std::shared_ptr<Mesh>& mesh) {
        return mesh->mesh_id() == mesh_id;
      });
    auto mesh = *it;
    return mesh->vertex_buffer().maxCoeff() - mesh->vertex_buffer().minCoeff();
  }

  std::map<std::string, QuantizationInfo> Scene::quantize_updates(
    float relative_error_threshold,
    float absolute_error_threshold,
    const std::string& base_mesh_id,
    bool per_frame_range)
  {
    std::map<std::string, std::vector<std::shared_ptr<MeshUpdate>>> updates;
    for (auto& update : m_mesh_updates)
    {
      updates[update->base_mesh_id()].push_back(update);
    }

    std::map<std::string, QuantizationInfo> info;
    for (auto& mesh_updates : updates)
    {
      if (!base_mesh_id.empty() && mesh_updates.first != base_mesh_id)
      {
        continue;
      }

      float mesh_range = this->compute_mesh_range(mesh_updates.first);
      float representable_range = compute_representable_range(
        relative_error_threshold, absolute_error_threshold, mesh_range);
      info[mesh_updates.first] = quantize_updates_for_mesh(
        representable_range, mesh_updates.second, per_frame_range);
    }

    return info;
  }

} // namespace scenepic