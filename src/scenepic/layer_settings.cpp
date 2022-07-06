// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "layer_settings.h"

#include "util.h"

namespace scenepic
{
  LayerSettings::LayerSettings(
    bool filled, bool wireframe, double opacity, int render_order)
  : m_render_order(render_order),
    m_filled(filled),
    m_wireframe(wireframe),
    m_opacity(opacity)
  {}

  std::string LayerSettings::to_string() const
  {
    return this->to_json().to_string();
  }

  JsonValue LayerSettings::to_json() const
  {
    JsonValue obj;
    obj["filled"] = m_filled;
    obj["wireframe"] = m_wireframe;
    obj["opacity"] = m_opacity;
    if (m_render_order != LayerSettings::NO_RENDER_ORDER)
    {
      obj["renderOrder"] = static_cast<std::int64_t>(m_render_order);
    }

    return obj;
  }

  bool LayerSettings::filled() const
  {
    return m_filled;
  }

  LayerSettings& LayerSettings::filled(bool value)
  {
    m_filled = value;
    return *this;
  }

  bool LayerSettings::wireframe() const
  {
    return m_wireframe;
  }

  LayerSettings& LayerSettings::wireframe(bool value)
  {
    m_wireframe = value;
    return *this;
  }

  double LayerSettings::opacity() const
  {
    return m_opacity;
  }

  LayerSettings& LayerSettings::opacity(double value)
  {
    m_opacity = value;
    return *this;
  }

  int LayerSettings::render_order() const
  {
    return m_render_order;
  }

  LayerSettings& LayerSettings::render_order(int value)
  {
    m_render_order = value;
    return *this;
  }

  const int LayerSettings::NO_RENDER_ORDER = std::numeric_limits<int>::max();

} // namespace scenepic