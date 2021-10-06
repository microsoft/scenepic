// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#ifndef _SCENEPIC_LABEL_H_
#define _SCENEPIC_LABEL_H_

#include "color.h"
#include "json_value.h"
#include "mesh.h"

#include <cstdint>
#include <string>

namespace scenepic
{
  /** A ScenePic label type */
  class Label
  {
  public:
    /**  A unique identifier for the Label */
    const std::string& label_id() const;

    /** The mesh associated with this label */
    const std::string& mesh_id() const;

    /** The text to use in the label. */
    const std::string& text() const;

    /** The text to use in the label. */
    Label& text(const std::string& value);

    /** The color to use */
    const Color& fill_color() const;

    /** The color to use */
    Label& fill_color(const Color& value);

    /** The size in pixels to display the label at */
    double size_in_pixels() const;

    /** The size in pixels to display the label at */
    Label& size_in_pixels(double value);

    /** HTML font family name. */
    const std::string& font_family() const;

    /** HTML font family name. */
    Label& font_family(const std::string& value);

    /** How to align the label horizontally (one of 'left', 'center', 'right')
     */
    const std::string& horizontal_align() const;

    /** How to align the label horizontally (one of 'left', 'center', 'right')
     */
    Label& horizontal_align(const std::string& value);

    /** How to align the label vertically (one of 'top', 'middle', 'bottom') */
    const std::string& vertical_align() const;

    /** How to align the label vertically (one of 'top', 'middle', 'bottom') */
    Label& vertical_align(const std::string& value);

    /** Distance in world space to offset the label by when using
     * left/right/top/bottom alignment. */
    double offset_distance() const;

    /** Distance in world space to offset the label by when using
     * left/right/top/bottom alignment. */
    Label& offset_distance(double value);

    /** Whether this Mesh is defined in camera space
     *  (cannot be moved in the ScenePic user interface) or
     *  world space (standard).
     */
    bool camera_space() const;

    /** Whether this Mesh is defined in camera space
     *  (cannot be moved in the ScenePic user interface) or
     *  world space (standard).
     */
    Label& camera_space(bool camera_space);

    /** An identifier to allow several Meshes to be grouped
     *  together into a "layer".  Layers should be used in
     *  conjunction with Canvas3D.set_layer_settings().
     *  \see Canvas3D::set_layer_settings.
     */
    const std::string& layer_id() const;

    /** An identifier to allow several Meshes to be grouped
     *  together into a "layer".  Layers should be used in
     *  conjunction with Canvas3D.set_layer_settings().
     *  \see Canvas3D::set_layer_settings.
     */
    Label& layer_id(const std::string& layer_id);

    /** Return a JSON string representing the object */
    std::string to_string() const;

    /** Convert this object into ScenePic json.
     *  \return a json value
     */
    JsonValue to_json() const;

  private:
    friend class Scene;

    /**
     * Constructor.
     * \param label_id a unique identifier for the Label (will be automatically
     *                 populated if not provided).
     * \param mesh pointer to the Mesh object used to render the label
     */
    Label(const std::string& label_id, const std::shared_ptr<Mesh>& mesh);

    std::string m_label_id;
    std::string m_text;
    double m_size_in_pixels;
    std::string m_font_family;
    std::string m_horizontal_align;
    std::string m_vertical_align;
    double m_offset_distance;
    std::shared_ptr<Mesh> m_mesh;
  };
} // namespace scenepic

#endif