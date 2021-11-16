// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "frame3d.h"

#include "transforms.h"
#include "util.h"

namespace scenepic
{
  Frame3D::Frame3D(
    const std::string& frame_id,
    const FocusPoint& focus_point,
    const Camera& camera)
  : m_frame_id(frame_id), m_focus_point(focus_point), m_camera(camera)
  {}

  void Frame3D::add_meshes_by_id(
    const std::vector<std::string>& mesh_ids, const Transform& transform)
  {
    for (const auto& mesh_id : mesh_ids)
    {
      this->add_mesh_by_id(mesh_id, transform);
    }
  }

  void Frame3D::add_mesh_by_id(
    const std::string& mesh_id, const Transform& transform)
  {
    JsonValue instance;
    instance["CommandType"] = "AddMesh";
    instance["MeshId"] = mesh_id;
    if (!transform.isIdentity())
    {
      instance["Transform"] = matrix_to_json(transform);
    }

    m_frame_commands.push_back(instance);
  }

  void Frame3D::add_label(
    const std::shared_ptr<Label>& label, const Vector& position)
  {
    this->add_mesh_by_id(label->mesh_id(), Transforms::translate(position));
  }

  void Frame3D::set_layer_settings(
    const std::map<std::string, LayerSettings> layer_settings)
  {
    m_layer_settings = layer_settings;
  }

  JsonValue Frame3D::to_json() const
  {
    JsonValue obj;

    JsonValue command;
    command["CommandType"] = "AddFrame";
    command["FrameId"] = m_frame_id;

    JsonValue frame_commands;
    frame_commands["CommandType"] = "FrameCommands";
    frame_commands["FrameId"] = m_frame_id;
    frame_commands["Commands"].resize(0);
    for (auto frame_command : m_frame_commands)
    {
      frame_commands["Commands"].append(frame_command);
    }

    if (!m_focus_point.is_none())
    {
      frame_commands["Commands"].append(m_focus_point.to_json());
    }

    if (!m_camera.is_none())
    {
      frame_commands["Commands"].append(m_camera.to_json());
    }

    if (m_layer_settings.size())
    {
      JsonValue layer_settings;
      layer_settings["CommandType"] = "SetLayerSettings";
      for (const auto& layer : m_layer_settings)
      {
        layer_settings["Value"][layer.first] = layer.second.to_json();
      }

      frame_commands["Commands"].append(layer_settings);
    }

    obj.append(command);
    obj.append(frame_commands);

    return obj;
  }

  std::string Frame3D::to_string() const
  {
    return this->to_json().to_string();
  }

  Frame3D& Frame3D::camera(const Camera& camera)
  {
    m_camera = camera;
    return *this;
  }

  const Camera& Frame3D::camera() const
  {
    return m_camera;
  }

  const FocusPoint& Frame3D::focus_point() const
  {
    return m_focus_point;
  }

  Frame3D& Frame3D::focus_point(const FocusPoint& value)
  {
    m_focus_point = value;
    return *this;
  }

} // namespace scenepic