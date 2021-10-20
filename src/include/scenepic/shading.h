// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#ifndef _SCENEPIC_PARAMETERS_H_
#define _SCENEPIC_PARAMETERS_H_

#include "color.h"
#include "json_value.h"
#include "matrix.h"

namespace scenepic
{
  /** Parameters of the shaders */
  class Shading
  {
  public:
    /** Constructor.
     *  \param bg_color background color
     *  \param ambient_light_color color of ambient light
     *  \param directional_light_color color of directional light
     *  \param directional_light_dir direction of directional light
     */
    Shading(
      const Color& bg_color = Color(0, 0, 0),
      const Color& ambient_light_color = Color(0.7f, 0.7f, 0.7f),
      const Color& directional_light_color = Color(0.3f, 0.3f, 0.3f),
      const Vector& directional_light_dir = Vector(2, 1, 2));

    /** Constructor.
     *  \param bg_color background color (RGBA)
     *  \param ambient_light_color color of ambient light
     *  \param directional_light_color color of directional light
     *  \param directional_light_dir direction of directional light
     */
    Shading(
      const Color4& bg_color,
      const Color& ambient_light_color,
      const Color& directional_light_color,
      const Vector& directional_light_dir);

    const Color4& bg_color() const;

    const Color& ambient_light_color() const;

    const Color& directional_light_color() const;

    const Vector& directional_light_dir() const;

    /** Return a JSON string representing the object */
    std::string to_string() const;

    /** Convert this object into ScenePic json.
     *  \return a json value
     */
    JsonValue to_json() const;

    /** Whether this is a "no shading" instance */
    bool is_none() const;

    /** A Shading value indication "no shading" */
    static const Shading None();

  private:
    Color4 m_bg_color;
    Color m_ambient_light_color;
    Color m_directional_light_color;
    Vector m_directional_light_dir;
  };
} // namespace scenepic

#endif