// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#ifndef _SCENEPIC_LAYER_SETTINGS_H_
#define _SCENEPIC_LAYER_SETTINGS_H_

#include "json_value.h"

#include <limits>
#include <string>

namespace scenepic
{
  /** Settings used for customizing canvas drawing by layer */
  class LayerSettings
  {
  public:
    /** Constructor.
     *
     * \param render_order Where in the transparency order to draw this
     *                     layer
     * \param filled whether to draw the layer with filled polygons
     * \param wireframe whether to draw the layer with a frame outline
     * \param opacity the opacity with which to draw the layer
     */
    LayerSettings(
      bool filled = true,
      bool wireframe = false,
      double opacity = 1.0,
      int render_order = LayerSettings::NO_RENDER_ORDER);

    /** Specifies whether to draw the layer with filled triangles */
    bool filled() const;

    /** Specifies whether to draw the layer with filled triangles */
    LayerSettings& filled(bool value);

    /** Specifies whether to draw the layer with a wireframe outline */
    bool wireframe() const;

    /** Specifies whether to draw the layer with a wireframe outline */
    LayerSettings& wireframe(bool value);

    /** In [0,1] specifies the opacity with which to draw the layer */
    double opacity() const;

    /** In [0,1] specifies the opacity with which to draw the layer */
    LayerSettings& opacity(double value);

    /** Specifies a fixed order to draw transparency. */
    int render_order() const;

    /** Specifies a fixed order to draw transparency. */
    LayerSettings& render_order(int value);

    /** Return a JSON string representing the object */
    std::string to_string() const;

    /** Convert this object into ScenePic json.
     *  \return a json value
     */
    JsonValue to_json() const;

    /** Special value indicating that this layer has no specific rendering order
     */
    static const int NO_RENDER_ORDER;

  private:
    int m_render_order;
    bool m_filled;
    bool m_wireframe;
    double m_opacity;
  };
} // namespace scenepic

#endif