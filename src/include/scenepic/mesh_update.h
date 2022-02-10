// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#ifndef _SCENEPIC_MESH_UPDATE_H_
#define _SCENEPIC_MESH_UPDATE_H_

#include "json_value.h"
#include "matrix.h"

#include <cstdint>
#include <limits>
#include <string>
#include <vector>

namespace scenepic
{
  /** Flags indicating what aspect of the vertex buffer is updated. */
  enum class VertexBufferType
  {
    None = 0,
    Positions = 1,
    Normals = 2,
    Colors = 4,
    Rotations = 8
  };

  inline VertexBufferType operator|(VertexBufferType a, VertexBufferType b)
  {
    return (VertexBufferType)((int)a | (int)b);
  }

  inline VertexBufferType operator&(VertexBufferType a, VertexBufferType b)
  {
    return (VertexBufferType)((int)a & (int)b);
  }

  inline VertexBufferType& operator|=(VertexBufferType& a, VertexBufferType b)
  {
    return (VertexBufferType&)((int&)a |= (int)b);
  }

  /** Class which represents an update to an existing mesh in which only the
   *  vertex buffer is changed. By only updating a mesh the ScenePic file can
   *  become smaller, due to only needing to store the vertex buffer instead of
   *  the full mesh topology.
   */
  class MeshUpdate
  {
  public:
    /** The unique identifier of the original base mesh */
    const std::string& base_mesh_id() const;

    /** The unique identifier of the newly updated mesh */
    const std::string& mesh_id() const;

    /** The updated vertex buffer */
    VertexBufferRef vertex_buffer();

    /** Return a JSON string representing the object */
    std::string to_string() const;

    /** Convert this object into ScenePic json.
     *  \return a json value
     */
    JsonValue to_json() const;

    /** The unique index of the frame or the index of its keyframe (if
     * quantized).*/
    std::uint32_t frame_index() const;

    /** Whether this update is quantized. */
    bool is_quantized() const;

    /** Quantize the mesh update in reference to a keyframe.
     *  \param keyframe_index the index of the keyframe
     *  \param fixed_point_range the range to use for the fixed point
     *                           representation
     *  \param keyframe_vertex_buffer the keyframe vertex buffer
     */
    void quantize(
      std::uint32_t keyframe_index,
      float fixed_point_range,
      const ConstVertexBufferRef& keyframe_vertex_buffer);

    /** Unquantize the buffer (for testing purposes) */
    VertexBuffer unquantize() const;

    /** The range of differences between this frame and the current keyframe.
     *  \param vertex_buffer the keyframe vertex buffer
     *  \return the range in difference values
     */
    float difference_range(const ConstVertexBufferRef& vertex_buffer) const;

    /** The number of quantization bins. */
    static const std::size_t QuantizationBinCount =
      std::numeric_limits<FixedPointVertexBuffer::Scalar>::max();

  private:
    friend class Scene;

    /** Constructor.
     *
     * \param base_mesh_id the unique identifier of the original base mesh
     * \param mesh_id the unique identifier of the new mesh.
     * \param buffers the updated vertex buffers
     * \param buffer_types the types of the buffers
     * \param frame_index the unique index of the frame
     * \param updated the parts of the mesh that have been updated
     */
    MeshUpdate(
      const std::string& base_mesh_id,
      const std::string& mesh_id,
      const std::vector<ConstVertexBufferRef>& buffers,
      const std::vector<VertexBufferType>& buffer_types,
      std::uint32_t frame_index);

    std::string m_base_mesh_id;
    std::string m_mesh_id;
    VertexBuffer m_vertex_buffer;
    FixedPointVertexBuffer m_fp_vertex_buffer;
    float m_min;
    float m_max;
    std::uint32_t m_frame_index;
    std::uint32_t m_keyframe_index;
    VertexBufferType m_update_flags;
  };
} // namespace scenepic

#endif