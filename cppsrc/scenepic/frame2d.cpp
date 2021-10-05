// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "frame2d.h"

#include "util.h"

namespace scenepic
{
  Frame2D::Frame2D(const std::string& frame_id) : m_frame_id(frame_id) {}

  void Frame2D::add_line(
    const CoordinateBuffer& coordinates,
    const Color& line_color,
    float line_width,
    const Color& fill_color,
    bool close_path,
    const std::string& layer_id)
  {
    m_line_layer_ids.push_back(layer_id);
    std::uint16_t start = this->num_coordinates();
    append_matrix(m_coord_buffer, coordinates);
    std::uint16_t end = this->num_coordinates();
    Style style;
    style << line_color.is_none(), line_color.R(), line_color.G(),
      line_color.B(), fill_color.is_none(), fill_color.R(), fill_color.G(),
      fill_color.B();
    append_row(m_line_style_buffer, style);
    m_line_width.push_back(line_width);
    append_row(m_line_buffer, PolyLine(start, end, close_path));
  }

  void Frame2D::add_rectangle(
    float x,
    float y,
    float w,
    float h,
    const Color& line_color,
    float line_width,
    const Color& fill_color,
    const std::string& layer_id)
  {
    CoordinateBuffer coordinates(4, 2);
    coordinates << x, y, x + w, y, x + w, y + h, x, y + h;
    this->add_line(
      coordinates, line_color, line_width, fill_color, true, layer_id);
  }

  void Frame2D::add_circle(
    float x,
    float y,
    float radius,
    const Color& line_color,
    float line_width,
    const Color& fill_color,
    const std::string& layer_id)
  {
    m_circle_layer_ids.push_back(layer_id);
    std::uint32_t index = static_cast<std::uint32_t>(m_circle_buffer.rows());
    append_row(m_circle_buffer, Circle(x, y, radius, line_width));
    Style style;
    style << line_color.is_none(), line_color.R(), line_color.G(),
      line_color.B(), fill_color.is_none(), fill_color.R(), fill_color.G(),
      fill_color.B();
    append_row(m_circle_style_buffer, style);
  }

  void Frame2D::add_image(
    const std::string& image_id,
    const std::string& position_type,
    float x,
    float y,
    float scale,
    bool smoothed,
    const std::string& layer_id)
  {
    std::uint16_t index = this->num_coordinates();
    append_row(m_coord_buffer, Coordinate(x, y));
    Frame2D::Image image(
      image_id, position_type, index, scale, smoothed, layer_id);
    m_frame_commands.push_back(image.to_json());
  }

  void Frame2D::add_video(
    const std::string& position_type,
    float x,
    float y,
    float scale,
    bool smoothed,
    const std::string& layer_id)
  {
    std::uint16_t index = this->num_coordinates();
    append_row(m_coord_buffer, Coordinate(x, y));
    Frame2D::Video video(position_type, index, scale, smoothed, layer_id);
    m_frame_commands.push_back(video.to_json());
  }

  void Frame2D::add_text(
    const std::string& text,
    float left,
    float bottom,
    const Color& color,
    float size_in_pixels,
    const std::string& font_family,
    const std::string& layer_id)
  {
    std::uint16_t index = this->num_coordinates();
    append_row(m_coord_buffer, Coordinate(left, bottom));
    Frame2D::Text element(
      text, index, color, size_in_pixels, font_family, layer_id);
    m_frame_commands.push_back(element.to_json());
  }

  std::uint16_t Frame2D::num_coordinates() const
  {
    return static_cast<std::uint16_t>(m_coord_buffer.rows());
  }

  typedef Eigen::Matrix<std::uint8_t, 1, Eigen::Dynamic, Eigen::RowMajor>
    LineCloseBuffer;

  JsonValue run_length_encode(const std::vector<std::string>& layer_ids)
  {
    JsonValue command;
    command.resize(0);
    std::string current = layer_ids[0];
    std::int64_t count = 0;
    for (const auto& layer_id : layer_ids)
    {
      if (layer_id == current)
      {
        count += 1;
      }
      else
      {
        JsonValue id_count;
        id_count.resize(0);
        id_count.append(current);
        id_count.append(count);
        command.append(id_count);
        current = layer_id;
        count = 1;
      }
    }

    if (command.values().size() == 0 && current.empty())
    {
      return JsonValue::nullSingleton();
    }

    JsonValue id_count;
    id_count.resize(0);
    id_count.append(current);
    id_count.append(count);
    command.append(id_count);
    return command;
  }

  JsonValue Frame2D::to_json() const
  {
    JsonValue obj;

    JsonValue command;
    command["CommandType"] = "AddFrame";
    command["FrameId"] = m_frame_id;

    JsonValue frame_commands;
    frame_commands["CommandType"] = "FrameCommands";
    frame_commands["FrameId"] = m_frame_id;

    frame_commands["Commands"].resize(0);

    JsonValue set_coords;
    set_coords["CommandType"] = "SetCoordinates";
    set_coords["CoordinateBuffer"] = matrix_to_json(m_coord_buffer);
    frame_commands["Commands"].append(set_coords);

    for (auto& frame_command : m_frame_commands)
    {
      frame_commands["Commands"].append(frame_command);
    }

    if (m_line_buffer.size())
    {
      JsonValue add_lines;
      add_lines["CommandType"] = "AddLines";
      add_lines["InfoBuffer"] = matrix_to_json(m_line_buffer);
      add_lines["StyleBuffer"] = matrix_to_json(m_line_style_buffer);
      Eigen::Index cols = static_cast<Eigen::Index>(m_line_width.size());
      Eigen::Map<const Eigen::VectorXf> line_width_buffer(
        m_line_width.data(), cols);
      add_lines["WidthBuffer"] = matrix_to_json(line_width_buffer);
      JsonValue layer_ids = run_length_encode(m_line_layer_ids);
      if (layer_ids.type() != JsonType::Null)
      {
        add_lines["LayerIds"] = layer_ids;
      }

      frame_commands["Commands"].append(add_lines);
    }

    if (m_circle_buffer.size())
    {
      JsonValue add_circles;
      add_circles["CommandType"] = "AddCircles";
      add_circles["InfoBuffer"] = matrix_to_json(m_circle_buffer);
      add_circles["StyleBuffer"] = matrix_to_json(m_circle_style_buffer);
      JsonValue layer_ids = run_length_encode(m_circle_layer_ids);
      if (layer_ids.type() != JsonType::Null)
      {
        add_circles["LayerIds"] = layer_ids;
      }

      frame_commands["Commands"].append(add_circles);
    }

    obj.append(command);
    obj.append(frame_commands);

    return obj;
  }

  Frame2D::Text::Text(
    const std::string& text,
    std::uint32_t index,
    const Color& color,
    float size_in_pixels,
    const std::string& font_family,
    const std::string& layer_id)
  : m_text(text),
    m_index(index),
    m_fill_style(color.to_html_hex()),
    m_size_in_pixels(size_in_pixels),
    m_font_family(font_family),
    m_layer_id(layer_id)
  {}

  JsonValue Frame2D::Text::to_json() const
  {
    JsonValue obj;
    obj["CommandType"] = "AddText";
    obj["Text"] = m_text;
    obj["Index"] = static_cast<std::int64_t>(m_index);
    obj["FillStyle"] = m_fill_style;
    obj["Font"] = m_font_family;
    obj["SizeInPixels"] = m_size_in_pixels;
    if (!m_layer_id.empty())
    {
      obj["LayerId"] = m_layer_id;
    }

    return obj;
  }

  Frame2D::Image::Image(
    const std::string& image_id,
    const std::string& position_type,
    std::uint32_t index,
    float scale,
    bool smoothed,
    const std::string& layer_id)
  : m_image_id(image_id),
    m_position_type(position_type),
    m_index(index),
    m_scale(scale),
    m_smoothed(smoothed),
    m_layer_id(layer_id)
  {}

  JsonValue Frame2D::Image::to_json() const
  {
    JsonValue obj;
    obj["CommandType"] = "AddImage";
    obj["ImageId"] = m_image_id;
    obj["PositionType"] = m_position_type;
    if (m_position_type == "manual")
    {
      obj["Index"] = static_cast<std::int64_t>(m_index);
    }
    else
    {
      obj["Index"] = JsonValue::nullSingleton();
    }
    obj["Scale"] = m_scale;
    obj["Smoothed"] = m_smoothed;
    if (!m_layer_id.empty())
    {
      obj["LayerId"] = m_layer_id;
    }

    return obj;
  }

  Frame2D::Video::Video(
    const std::string& position_type,
    std::uint32_t index,
    float scale,
    bool smoothed,
    const std::string& layer_id)
  : m_position_type(position_type),
    m_index(index),
    m_scale(scale),
    m_smoothed(smoothed),
    m_layer_id(layer_id)
  {}

  JsonValue Frame2D::Video::to_json() const
  {
    JsonValue obj;
    obj["CommandType"] = "AddVideo";
    obj["PositionType"] = m_position_type;
    if (m_position_type == "manual")
    {
      obj["Index"] = static_cast<std::int64_t>(m_index);
    }
    else
    {
      obj["Index"] = JsonValue::nullSingleton();
    }
    obj["Scale"] = m_scale;
    obj["Smoothed"] = m_smoothed;
    if (!m_layer_id.empty())
    {
      obj["LayerId"] = m_layer_id;
    }

    return obj;
  }

  std::string Frame2D::to_string() const
  {
    return this->to_json().to_string();
  }

} // namespace scenepic