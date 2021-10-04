// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "mesh_update.h"
#include "base64.h"
#include "util.h"

namespace {
    std::uint32_t NO_KEYFRAME = 0xFFFFFFFF;
    std::uint16_t MAX_FIXED = 0xFFFF;
}

namespace scenepic
{

MeshUpdate::MeshUpdate(const std::string &base_mesh_id,
                       const std::string &mesh_id,
                       const ConstVertexBufferRef &vertex_buffer,
                       std::uint32_t frame_index) :
    m_base_mesh_id(base_mesh_id),
    m_mesh_id(mesh_id),
    m_vertex_buffer(vertex_buffer),
    m_fp_vertex_buffer(FixedPointVertexBuffer::Zero(0, 6)),
    m_frame_index(frame_index),
    m_min(0),
    m_max(0),
    m_keyframe_index(NO_KEYFRAME)
{
}

const std::string &MeshUpdate::base_mesh_id() const
{
    return m_base_mesh_id;
}

const std::string &MeshUpdate::mesh_id() const
{
    return m_mesh_id;
}

VertexBufferRef MeshUpdate::vertex_buffer()
{
    return VertexBufferRef(m_vertex_buffer);
}

void MeshUpdate::quantize(std::uint32_t keyframe_index, float fixed_point_range, const ConstVertexBufferRef &keyframe_vertex_buffer)
{
    m_keyframe_index = keyframe_index;
    VertexBuffer diff = m_vertex_buffer - keyframe_vertex_buffer;
    m_min = diff.minCoeff();
    m_max = diff.maxCoeff();
    assert(m_max - m_min <= fixed_point_range);

    float center = 0.5f * (m_min + m_max);
    m_min = center - 0.5f * fixed_point_range;
    m_max = center + 0.5f * fixed_point_range;

    diff = (diff.array() - m_min).matrix();
    float scale = MAX_FIXED / fixed_point_range;
    diff = diff * scale;
    m_fp_vertex_buffer = diff.cast<std::uint16_t>();
}

VertexBuffer MeshUpdate::unquantize() const
{
    float scale = (m_max - m_min) / MAX_FIXED;
    VertexBuffer buffer = m_fp_vertex_buffer.cast<float>() * scale;
    buffer = (buffer.array() + m_min).matrix();
    return buffer;
}

float MeshUpdate::difference_range(const ConstVertexBufferRef &vertex_buffer) const
{
    auto diff = m_vertex_buffer - vertex_buffer;
    return diff.maxCoeff() - diff.minCoeff();
}

bool MeshUpdate::is_quantized() const
{
    return m_keyframe_index != NO_KEYFRAME;
}

JsonValue MeshUpdate::to_json() const
{
    JsonValue obj;
    obj["CommandType"] = "UpdateMesh";
    obj["BaseMeshId"] = m_base_mesh_id;
    obj["MeshId"] = m_mesh_id;
    if(this->is_quantized())
    {
        obj["FrameIndex"] = static_cast<std::int64_t>(m_frame_index);
        obj["KeyframeIndex"] = static_cast<std::int64_t>(m_keyframe_index);
        obj["MinValue"] = m_min;
        obj["MaxValue"] = m_max;
        obj["QuantizedBuffer"] = matrix_to_json(m_fp_vertex_buffer);
    }
    else
    {
        obj["FrameIndex"] = static_cast<std::int64_t>(m_frame_index);
        obj["VertexBuffer"] = matrix_to_json(m_vertex_buffer);
    }

    return obj;
}

std::uint32_t MeshUpdate::frame_index() const
{
    return m_frame_index;
}

std::string MeshUpdate::to_string() const
{
    return this->to_json().to_string();
}
}