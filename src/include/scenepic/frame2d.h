// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#ifndef _SCENEPIC_FRAME2D_H_
#define _SCENEPIC_FRAME2D_H_

#include "color.h"
#include "json_value.h"
#include "matrix.h"

#include <map>
#include <string>

namespace scenepic
{
  /** Represents a frame of a 2D animation */

  class Frame2D
  {
  public:
    /** Add a line to the frame.
     *  \param coordinates the coordinates of the line
     *  \param line_color the color of the line
     *  \param line_width the width of the line
     *  \param fill_color the fill color to use
     *  \param close_path whether to close the path
     *  \param layer_id the unique ID of the layer for this primitive
     */
    void add_line(
      const CoordinateBuffer& coordinates,
      const Color& line_color = Colors::Black,
      float line_width = 1.0f,
      const Color& fill_color = Color::None(),
      bool close_path = false,
      const std::string& layer_id = "");

    /** Add a rectangle to the frame.
     *  \param x the upper left corner x coordinate
     *  \param y the upper left corner y coordinate
     *  \param w the width of the rectangle
     *  \param h the height of the rectangle
     *  \param line_color the color of the line
     *  \param line_width the width of the line
     *  \param fill_color the fill color to use
     *  \param layer_id the unique ID of the layer for this primitive
     */
    void add_rectangle(
      float x,
      float y,
      float w,
      float h,
      const Color& line_color = Colors::Black,
      float line_width = 1.0f,
      const Color& fill_color = Color::None(),
      const std::string& layer_id = "");

    /** Add a circle to the frame.
     *  \param x the center x coordinate
     *  \param y the center y coordinate
     *  \param radius the radius of the circle
     *  \param line_color the color of the line
     *  \param line_width the width of the line
     *  \param fill_color the fill color to use
     *  \param layer_id the unique ID of the layer for this primitive
     */
    void add_circle(
      float x,
      float y,
      float radius,
      const Color& line_color = Colors::Black,
      float line_width = 1.0f,
      const Color& fill_color = Color::None(),
      const std::string& layer_id = "");

    /** Add an image to the frame.
     *  \param image_id the unique identifier of the image
     *  \param position_type one of "fit", "fill", "stretch", or "manual"
     *  \param x x coordinate to draw image at if position_type == "manual".
     *  \param y y coordinate to draw image at if position_type == "manual".
     *  \param scale scale factor to draw image by if position_type == "manual".
     *  \param smoothed whether to draw an image with smoothing or using nearest
     *                  neighbor.
     *  \param layer_id the unique ID of the layer for this primitive
     */
    void add_image(
      const std::string& image_id,
      const std::string& position_type = "fit",
      float x = 0.0f,
      float y = 0.0f,
      float scale = 1.0f,
      bool smoothed = false,
      const std::string& layer_id = "");

    /** Add a video to the frame. The image shown will be the corresponding
     *  frame from the video.
     *  \param position_type one of "fit", "fill", "stretch", or "manual"
     *  \param x x coordinate to draw the video frame at if position_type ==
     *           "manual".
     *  \param y y coordinate to draw the video frame at if position_type ==
     *           "manual".
     *  \param scale scale factor to draw the video frame by if position_type
     *               == "manual".
     *  \param smoothed whether to draw the video frame with smoothing or using
     *                  nearest neighbor.
     *  \param layer_id the unique ID of the layer for this primitive
     */
    void add_video(
      const std::string& position_type = "fit",
      float x = 0.0f,
      float y = 0.0f,
      float scale = 1.0f,
      bool smoothed = false,
      const std::string& layer_id = "");

    /** Add some text to the frame.
     *  \param text the text to display
     *  \param left the pixel position of the left side of the text
     *  \param bottom the pixel position of the bottom of the text
     *  \param color the color of the text
     *  \param size_in_pixels the vertical size of the text in pixels
     *  \param font_family the font to use for the text
     *  \param layer_id the unique ID of the layer for this primitive
     */
    void add_text(
      const std::string& text,
      float left,
      float bottom,
      const Color& color = Colors::White,
      float size_in_pixels = 12.0f,
      const std::string& font_family = "sans-serif",
      const std::string& layer_id = "");

    /** The number of coordinates in the buffer. */
    std::uint16_t num_coordinates() const;

    /** Return a JSON string representing the object */
    std::string to_string() const;

    /** Convert this object into ScenePic json.
     *  \return a json value
     */
    JsonValue to_json() const;

  private:
    friend class Canvas2D;

    /** Constructor.
     *  \param frame_id the unique identifier for this frame
     */
    Frame2D(const std::string& frame_id);

    IndexVector
    lookup_layer_ids(const std::vector<std::string>& layer_ids) const;
    std::map<std::string, std::uint8_t> m_layer_lookup;

    std::string m_frame_id;
    CoordinateBuffer m_coord_buffer;

    PolyLineBuffer m_line_buffer;
    StyleBuffer m_line_style_buffer;
    std::vector<std::string> m_line_layer_ids;
    std::vector<float> m_line_width;

    CircleBuffer m_circle_buffer;
    std::vector<std::string> m_circle_layer_ids;
    StyleBuffer m_circle_style_buffer;

    std::vector<JsonValue> m_frame_commands;

    class Text
    {
    public:
      Text(
        const std::string& text,
        std::uint32_t index,
        const Color& color,
        float size_in_pixels,
        const std::string& font_family,
        const std::string& layer_id);

      JsonValue to_json() const;

    private:
      std::string m_text;
      std::uint32_t m_index;
      std::string m_fill_style;
      float m_size_in_pixels;
      std::string m_font_family;
      std::string m_layer_id;
    };

    class Image
    {
    public:
      Image(
        const std::string& image_id,
        const std::string& position_type,
        std::uint32_t index,
        float scale,
        bool smoothed,
        const std::string& layer_id);

      JsonValue to_json() const;

    private:
      std::string m_image_id;
      std::string m_position_type;
      std::uint32_t m_index;
      float m_scale;
      bool m_smoothed;
      std::string m_layer_id;
    };

    class Video
    {
    public:
      Video(
        const std::string& position_type,
        std::uint32_t index,
        float scale,
        bool smoothed,
        const std::string& layer_id);

      JsonValue to_json() const;

    private:
      std::string m_position_type;
      std::uint32_t m_index;
      float m_scale;
      bool m_smoothed;
      std::string m_layer_id;
    };
  };
} // namespace scenepic

#endif