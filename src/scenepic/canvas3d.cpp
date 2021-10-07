// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "canvas3d.h"

#include "util.h"

namespace scenepic
{
  Canvas3D::Canvas3D(
    const std::string& canvas_id,
    double width,
    double height,
    const Camera& camera,
    const Shading& shading,
    const FocusPoint& focus_point,
    const UIParameters& ui_parameters)
  : m_canvas_id(canvas_id),
    m_width(width),
    m_height(height),
    m_camera(camera),
    m_shading(shading),
    m_focus_point(focus_point),
    m_ui_parameters(ui_parameters),
    m_num_frames(0),
    m_media_id("")
  {}

  const std::string& Canvas3D::canvas_id() const
  {
    return m_canvas_id;
  }

  void Canvas3D::set_layer_settings(
    const std::map<std::string, LayerSettings> layer_settings)
  {
    m_layer_settings = layer_settings;
  }

  void Canvas3D::clear_script()
  {
    m_camera = Camera::None();
    m_shading = Shading::None();
    m_focus_point = FocusPoint::None();
    m_ui_parameters = UIParameters::None();
    m_layer_settings.clear();
    m_frames.clear();
  }

  JsonValue Canvas3D::to_json() const
  {
    JsonValue obj;

    JsonValue canvas_commands;
    canvas_commands.resize(0);
    if (!m_camera.is_none())
    {
      canvas_commands.append(m_camera.to_json());
    }

    if (!m_shading.is_none())
    {
      canvas_commands.append(m_shading.to_json());
    }

    if (!m_ui_parameters.is_none())
    {
      canvas_commands.append(m_ui_parameters.to_json());
    }

    if (!m_focus_point.is_none())
    {
      canvas_commands.append(m_focus_point.to_json());
    }

    if (!m_media_id.empty())
    {
      JsonValue media;
      media["CommandType"] = "SetMedia";
      media["MediaId"] = m_media_id;
      canvas_commands.append(media);
    }

    if (m_layer_settings.size())
    {
      JsonValue layer_settings;
      layer_settings["CommandType"] = "SetLayerSettings";
      for (const auto& layer : m_layer_settings)
      {
        layer_settings["Value"][layer.first] = layer.second.to_json();
      }

      canvas_commands.append(layer_settings);
    }

    for (const auto& frame : m_frames)
    {
      canvas_commands.append(frame->to_json());
    }

    obj["CommandType"] = "CanvasCommands";
    obj["CanvasId"] = m_canvas_id;
    obj["Commands"] = canvas_commands;
    return obj;
  }

  std::shared_ptr<Frame3D> Canvas3D::create_frame(
    const std::string& frame_id_init,
    const FocusPoint& focus_point,
    const std::vector<std::string>& mesh_ids,
    const Camera& camera)
  {
    std::string frame_id = frame_id_init;
    if (frame_id.size() == 0)
    {
      frame_id = std::to_string(m_num_frames);
    }

    auto frame =
      std::make_shared<Frame3D>(Frame3D(frame_id, focus_point, camera));
    m_frames.push_back(frame);
    m_num_frames += 1;

    if (mesh_ids.size())
    {
      frame->add_meshes_by_id(mesh_ids);
    }

    return frame;
  }

  const Camera& Canvas3D::camera() const
  {
    return m_camera;
  }

  Canvas3D& Canvas3D::camera(const Camera& value)
  {
    m_camera = value;
    float aspect_ratio = static_cast<float>(m_width / m_height);
    float diff = std::abs(aspect_ratio - m_camera.aspect_ratio());
    if (diff > 1e-3)
    {
      std::cerr << "Camera/Canvas aspect ratio mismatch for " << m_canvas_id
                << ": " << aspect_ratio << " != " << m_camera.aspect_ratio()
                << ". Forcing camera aspect ratio." << std::endl;
      m_camera.aspect_ratio(aspect_ratio);
    }

    return *this;
  }

  const Shading& Canvas3D::shading() const
  {
    return m_shading;
  }

  Canvas3D& Canvas3D::shading(const Shading& value)
  {
    m_shading = value;
    return *this;
  }

  const std::string& Canvas3D::media_id() const
  {
    return m_media_id;
  }

  Canvas3D& Canvas3D::media_id(const std::string& media_id)
  {
    m_media_id = media_id;
    return *this;
  }

  const FocusPoint& Canvas3D::focus_point() const
  {
    return m_focus_point;
  }

  Canvas3D& Canvas3D::focus_point(const FocusPoint& value)
  {
    m_focus_point = value;
    return *this;
  }

  const UIParameters& Canvas3D::ui_parameters() const
  {
    return m_ui_parameters;
  }

  Canvas3D& Canvas3D::ui_parameters(const UIParameters& value)
  {
    m_ui_parameters = value;
    return *this;
  }

  std::string Canvas3D::to_string() const
  {
    return this->to_json().to_string();
  }

  double Canvas3D::width() const
  {
    return m_width;
  }

  double Canvas3D::height() const
  {
    return m_height;
  }
} // namespace scenepic