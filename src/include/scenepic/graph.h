// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#ifndef _SCENEPIC_GRAPH_H_
#define _SCENEPIC_GRAPH_H_

#include "color.h"
#include "json_value.h"
#include "matrix.h"

#include <memory>
#include <string>
#include <vector>

namespace scenepic
{
  /** A 2D viewport that animates one or more sparklines. */
  class Graph
  {
  public:
    /** Represents the margin along the edges of a graph. */
    struct Margin
    {
      /** Default constructor. Sets all margins to 10 pixels. */
      Margin();

      /** Constructor.
       *  \param size space in pixels to use for all margins
       */
      Margin(double size);

      /** Constructor.
       *  \param top the top margin value in pixels
       *  \param right the right margin value in pixels
       *  \param bottom the bottom margin value in pixels
       *  \param left the left margin value in pixels
       */
      Margin(double top, double right, double bottom, double left);

      /** Returns a string representation of the margin */
      std::string to_string() const;

      /** Convert this object into ScenePic json.
       *  \return a json value
       */
      JsonValue to_json() const;

      /** The top margin in pixels */
      double top;

      /** The right margin in pixels */
      double right;

      /** The bottom margin in pixels */
      double bottom;

      /** The left margin in pixels */
      double left;
    };

    /** Represents a vertical line in a sparkline graph. */
    struct VerticalRule
    {
      /** Constructor.
       *  \param frame the frame at which to add the line
       *  \param color the color of the line
       *  \param line_width the width of the line in pixels
       */
      VerticalRule(
        std::int64_t frame,
        const Color& color = scenepic::Colors::Black,
        float line_width = 1.0f);

      /** Returns a string representation of the margin */
      std::string to_string() const;

      /** Convert this object into ScenePic json.
       *  \return a json value
       */
      JsonValue to_json() const;

      /** The frame at which to add the line. */
      std::int64_t frame;
      /** The color of the line. */
      Color color;
      /** The width of the line in pixels. */
      float line_width;
    };

    /** A unique identifier for the canvas */
    const std::string& canvas_id() const;

    /** Adds a sparkline to the graph.
     *  \param name the name of the measured quantity
     *  \param values a vector of measured values, one per frame
     *  \param line_color the color of the line (and its labels)
     *  \param line_width the width of the line
     *  \param vertical_rules vertical rules to draw on the sparkline
     */
    void add_sparkline(
      const std::string& name,
      const std::vector<float>& values,
      const scenepic::Color& line_color = scenepic::Colors::Black,
      float line_width = 1.0f,
      const std::vector<VerticalRule>& vertical_rules = {});

    /** Return a JSON string representing the object */
    std::string to_string() const;

    /** Convert this object into ScenePic json.
     *  \return a json value
     */
    JsonValue to_json() const;

    /** The background color of the frame */
    const Color& background_color() const;

    /** Set the background color of the frame */
    Graph& background_color(const Color& color);

    /** The outside margin of the graph */
    const Margin& margin() const;

    /** Set the outside margin of the graph */
    Graph& margin(const Margin& margin);

    /** How to align the sparkline label (one of 'left', 'right', 'top',
     * 'bottom', or 'none')
     */
    const std::string& name_align() const;

    /** How to align the sparkline label (one of 'left', 'right', 'top',
     * 'bottom', or 'none')
     */
    Graph& name_align(const std::string& name_align);

    /** How to align the sparkline value (one of 'left', 'right', 'top',
     * 'bottom', or 'none')
     */
    const std::string& value_align() const;

    /** How to align the sparkline value (one of 'left', 'right', 'top',
     * 'bottom', or 'none')
     */
    Graph& value_align(const std::string& value_align);

    /** The font family used for the graph labels */
    const std::string& font_family() const;

    /** Set the font family used for the graph labels */
    Graph& font_family(const std::string& font_family);

    /** The size of the graph labels in pixels */
    float name_size() const;

    /** Set the size of the graph labels in pixels */
    Graph& name_size(float name_size);

    /** The size of the graph labels in pixels */
    float value_size() const;

    /** Set the size of the graph labels in pixels */
    Graph& value_size(float value_size);

    /** The unique ID of the media file associated with this canvas.
     *  This file will be used to drive playback, i.e. frames will be displayed
     *  in time with of the media file.
     */
    const std::string& media_id() const;

    /** The unique ID of the media file associated with this canvas */
    Graph& media_id(const std::string& media_id);

  private:
    class Sparkline
    {
    public:
      Sparkline(
        const std::string& name,
        const ValueBuffer& values,
        const Color& color,
        float line_width,
        const std::vector<VerticalRule>& vertical_rules);

      std::string to_string() const;
      JsonValue to_json() const;

    private:
      std::string m_name;
      ValueBuffer m_values;
      Color m_color;
      float m_line_width;
      std::vector<VerticalRule> m_vertical_rules;
    };

    friend class Scene;

    /** Constructor.
     *  \param canvas_id a unique identifier for the Graph
     */
    Graph(const std::string& canvas_id);

    std::string m_canvas_id;
    std::string m_media_id;
    std::vector<Sparkline> m_sparklines;
    Color m_background_color;
    Margin m_margin;
    std::string m_font_family;
    std::string m_name_align;
    std::string m_value_align;
    float m_name_size;
    float m_value_size;
  };
} // namespace scenepic

#endif