// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "graph.h"

#include "util.h"

namespace scenepic
{
  Graph::Margin::Margin() : Margin(10) {}

  Graph::Margin::Margin(double size) : Margin(size, size, size, size) {}

  Graph::Margin::Margin(double top, double right, double bottom, double left)
  : top(top), right(right), bottom(bottom), left(left)
  {}

  std::string Graph::Margin::to_string() const
  {
    return this->to_json().to_string();
  }

  JsonValue Graph::Margin::to_json() const
  {
    JsonValue obj;
    obj["Top"] = this->top;
    obj["Right"] = this->right;
    obj["Bottom"] = this->bottom;
    obj["Left"] = this->left;
    return obj;
  }

  Graph::Sparkline::Sparkline(
    const std::string& name,
    const ValueBuffer& values,
    const Color& color,
    float line_width)
  : m_name(name), m_values(values), m_color(color), m_line_width(line_width)
  {}

  JsonValue Graph::Sparkline::to_json() const
  {
    JsonValue command;
    command["CommandType"] = "AddSparkline";
    command["ValueBuffer"] = matrix_to_json(m_values);
    command["Name"] = m_name;
    command["StrokeStyle"] = m_color.to_html_hex();
    command["LineWidth"] = m_line_width;
    return command;
  }

  const std::string& Graph::canvas_id() const
  {
    return m_canvas_id;
  }

  void Graph::add_sparkline(
    const std::string& name,
    const std::vector<float>& values,
    const scenepic::Color& color,
    float line_width)
  {
    ValueBuffer value_buffer(values.size(), 1);
    std::copy(values.begin(), values.end(), value_buffer.data());
    m_sparklines.emplace_back(name, value_buffer, color, line_width);
  }

  Graph::Graph(const std::string& canvas_id) : m_canvas_id(canvas_id) {}

  std::string Graph::to_string() const
  {
    return this->to_json().to_string();
  }

  JsonValue Graph::to_json() const
  {
    JsonValue obj;

    JsonValue canvas_commands;

    JsonValue margin;
    margin["CommandType"] = "SetMargin";
    margin["Value"] = m_margin.to_json();
    canvas_commands.append(margin);

    JsonValue background;
    background["CommandType"] = "SetBackgroundStyle";
    background["Value"] = m_background_color.to_html_hex();
    canvas_commands.append(background);

    JsonValue text;
    text["CommandType"] = "SetTextStyle";
    text["FontFamily"] = m_font_family;
    text["SizeInPixels"] = m_text_size;
    canvas_commands.append(text);

    for (const auto& sparkline : m_sparklines)
    {
      canvas_commands.append(sparkline.to_json());
    }

    if (!m_media_id.empty())
    {
      JsonValue media;
      media["CommandType"] = "SetMedia";
      media["MediaId"] = m_media_id;
      canvas_commands.append(media);
    }

    obj["CommandType"] = "CanvasCommands";
    obj["CanvasId"] = m_canvas_id;
    obj["Commands"] = canvas_commands;
    return obj;
  }

  const Color& Graph::background_color() const
  {
    return m_background_color;
  }

  Graph& Graph::background_color(const Color& color)
  {
    m_background_color = color;
    return *this;
  }

  const Graph::Margin& Graph::margin() const
  {
    return m_margin;
  }

  Graph& Graph::margin(const Graph::Margin& margin)
  {
    m_margin = margin;
    return *this;
  }

  const std::string& Graph::font_family() const
  {
    return m_font_family;
  }

  Graph& Graph::font_family(const std::string& font_family)
  {
    m_font_family = font_family;
    return *this;
  }

  float Graph::text_size() const
  {
    return m_text_size;
  }

  Graph& Graph::text_size(float text_size)
  {
    m_text_size = text_size;
    return *this;
  }

  const std::string& Graph::media_id() const
  {
    return m_media_id;
  }

  Graph& Graph::media_id(const std::string& media_id)
  {
    m_media_id = media_id;
    return *this;
  }
} // namespace scenepic