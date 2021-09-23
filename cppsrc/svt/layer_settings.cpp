#include "layer_settings.h"
#include "util.h"

namespace svt
{
    LayerSettings::LayerSettings(bool filled, bool wireframe, double opacity, int render_order)
        : m_render_order(render_order),
          m_filled(filled),
          m_wireframe(wireframe),
          m_opacity(opacity)
    {
    }

    std::string LayerSettings::to_string() const
    {
        return this->to_json().to_string();
    }

    JsonValue LayerSettings::to_json() const
    {
        JsonValue obj;
        obj["filled"] = this->m_filled;
        obj["wireframe"] = this->m_wireframe;
        obj["opacity"] = this->m_opacity;
        if (this->m_render_order != LayerSettings::NO_RENDER_ORDER)
        {
            obj["renderOrder"] = static_cast<std::int64_t>(this->m_render_order);
        }

        return obj;
    }

    bool LayerSettings::filled() const
    {
        return this->m_filled;
    }

    LayerSettings &LayerSettings::filled(bool value)
    {
        this->m_filled = value;
        return *this;
    }

    bool LayerSettings::wireframe() const
    {
        return this->m_wireframe;
    }

    LayerSettings &LayerSettings::wireframe(bool value)
    {
        this->m_wireframe = value;
        return *this;
    }

    double LayerSettings::opacity() const
    {
        return this->m_opacity;
    }

    LayerSettings &LayerSettings::opacity(double value)
    {
        this->m_opacity = value;
        return *this;
    }

    int LayerSettings::render_order() const
    {
        return this->m_render_order;
    }

    LayerSettings &LayerSettings::render_order(int value)
    {
        this->m_render_order = value;
        return *this;
    }

} // namespace svt