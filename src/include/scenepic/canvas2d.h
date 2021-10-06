// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#ifndef _SCENEPIC_CANVAS2D_H_
#define _SCENEPIC_CANVAS2D_H_

#include "color.h"
#include "frame2d.h"
#include "json_value.h"
#include "layer_settings.h"

#include <map>
#include <memory>
#include <string>
#include <vector>

namespace scenepic
{
  /** A 2D canvas that can contain a number of Frame2Ds. */
  class Canvas2D
  {
  public:
    /** A unique identifier for the canvas */
    const std::string& canvas_id() const;

    /** Creates a new Frame2D object and appends to the list of Frame2Ds in the
     *  Canvas2D.
     *  \param frame_id a unique identifier for the Frame3D (will be
     *                  automatically populated if not provided).
     *  \return a Frame2D object
     */
    std::shared_ptr<Frame2D> create_frame(const std::string& frame_id = "");

    /** Call this if you are dumping a ScenePic script in multiple parts,
     * e.g. for client-server interactions, or when chunking a large
     * visualization into multiple files.
     */
    void clear_script();

    /** Return a JSON string representing the object */
    std::string to_string() const;

    /** Convert this object into ScenePic json.
     *  \return a json value
     */
    JsonValue to_json() const;

    /** The background color of the frame */
    const Color& background_color() const;

    /** Set the background color of the frame */
    Canvas2D& background_color(const Color& color);

    /** The width of the canvas */
    double width() const;

    /** The width of the canvas */
    double height() const;

    /** The unique ID of the media file associated with this canvas.
     *  This file will be used to drive playback, i.e. frames will be displayed
     *  in time with the playback of the media file.
     */
    const std::string& media_id() const;

    /** Specify the visibilities and opacities of certain mesh layers.
     *  Each Mesh object can optionally be part of a user-identified layer
     *  (see Mesh constructor). Calling set_layer_settings will result in an
     *  additional drop-down selector in the ScenePic user interface.
     *  Currently, opacity is only guaranteed to be correct for Meshes
     *  that do not overlap. If you know one mesh should be rendered before
     *  another for opacity purposes, use the renderOrder setting:
     *  low numbers will be drawn before high numbers, and layers with no
     *  renderOrder set will be drawn first. Meshes without layer ids, or with
     *  layer ids that are not specified in this dictionary, will be drawn
     *  filled, without wireframe, at opacity 1.0, and before any layers with
     *  renderOrder set.
     */
    void set_layer_settings(
      const std::map<std::string, LayerSettings> layer_settings);

    Canvas2D& media_id(const std::string& media_id);

  private:
    friend class Scene;

    /** Constructor.
     *  \param canvas_id a unique identifier for the Canvas
     *  \param width the width of the Canvas
     *  \param height the height of the Canvas
     */
    Canvas2D(const std::string& canvas_id, double width, double height);

    std::string m_canvas_id;
    std::string m_media_id;
    std::vector<std::string> m_layer_ids;
    std::map<std::string, LayerSettings> m_layer_settings;
    std::map<std::string, std::uint8_t> m_layer_lookup;
    std::vector<std::shared_ptr<Frame2D>> m_frames;
    Color m_background_color;
    double m_width;
    double m_height;

    // This will get out of sync with the above after a call to clear_script()
    // DO NOT REMOVE - it is important
    std::size_t m_num_frames;
  };
} // namespace scenepic

#endif