// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "shading.h"

#include "util.h"

#include <limits>

namespace scenepic
{
  Shading::Shading(
    const Color& bg_color,
    const Color& ambient_light_color,
    const Color& directional_light_color,
    const Vector& directional_light_dir)
  : Shading(
      Color4(bg_color.r(), bg_color.g(), bg_color.b(), 1.0),
      ambient_light_color,
      directional_light_color,
      directional_light_dir)
  {}

  Shading::Shading(
    const Color4& bg_color,
    const Color& ambient_light_color,
    const Color& directional_light_color,
    const Vector& directional_light_dir)
  : m_bg_color(bg_color),
    m_ambient_light_color(ambient_light_color),
    m_directional_light_color(directional_light_color),
    m_directional_light_dir(directional_light_dir)
  {}

  const Color4& Shading::bg_color() const
  {
    return m_bg_color;
  }

  const Color& Shading::ambient_light_color() const
  {
    return m_ambient_light_color;
  }

  const Color& Shading::directional_light_color() const
  {
    return m_directional_light_color;
  }

  const Vector& Shading::directional_light_dir() const
  {
    return m_directional_light_dir;
  }

  JsonValue Shading::to_json() const
  {
    JsonValue obj;
    obj["CommandType"] = "SetShading";
    obj["Value"]["BackgroundColor"] = matrix_to_json(m_bg_color);
    obj["Value"]["AmbientLightColor"] = matrix_to_json(m_ambient_light_color);
    obj["Value"]["DirectionalLightColor"] =
      matrix_to_json(m_directional_light_color);
    obj["Value"]["DirectionalLightDir"] =
      matrix_to_json(m_directional_light_dir);
    return obj;
  }

  Color4 Color4None()
  {
    return Color4(
      -std::numeric_limits<Color::Scalar>::infinity(),
      -std::numeric_limits<Color::Scalar>::infinity(),
      -std::numeric_limits<Color::Scalar>::infinity(),
      -std::numeric_limits<Color::Scalar>::infinity());
  }

  bool Shading::is_none() const
  {
    return !(
      m_bg_color != Color4None() || m_ambient_light_color != Color::None() ||
      m_directional_light_color != Color::None() ||
      m_directional_light_dir != VectorNone());
  }

  const Shading Shading::None()
  {
    return Shading(Color4None(), Color::None(), Color::None(), VectorNone());
  }

  std::string Shading::to_string() const
  {
    return this->to_json().to_string();
  }
} // namespace scenepic
