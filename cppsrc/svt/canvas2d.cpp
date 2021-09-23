#include "canvas2d.h"
#include "util.h"

namespace svt
{
Canvas2D::Canvas2D(const std::string &canvas_id, double width, double height)
    : m_canvas_id(canvas_id),
      m_width(width),
      m_height(height),
      m_background_color(Colors::Black),
      m_num_frames(0),
      m_media_id("")
{
}

const std::string &Canvas2D::media_id() const
{
    return this->m_media_id;
}

Canvas2D& Canvas2D::media_id(const std::string &media_id)
{
    this->m_media_id = media_id;
    return *this;
}

const std::string &Canvas2D::canvas_id() const
{
    return this->m_canvas_id;
}

std::shared_ptr<Frame2D> Canvas2D::create_frame(const std::string &frame_id_init)
{
    std::string frame_id = frame_id_init;
    if (frame_id.size() == 0)
    {
        frame_id = std::to_string(this->m_num_frames);
    }

    auto frame = std::make_shared<Frame2D>(Frame2D(frame_id));
    this->m_frames.push_back(frame);
    this->m_num_frames += 1;
    return frame;
}

void Canvas2D::clear_script()
{
    this->m_frames.clear();
}

void Canvas2D::set_layer_settings(const std::map<std::string, LayerSettings> layer_settings)
{
    this->m_layer_settings = layer_settings;
    this->m_layer_ids.clear();
    this->m_layer_ids.push_back("");
    for(auto &layer : layer_settings)
    {
        this->m_layer_ids.push_back(layer.first);
    }
}

JsonValue Canvas2D::to_json() const
{
    JsonValue obj;

    JsonValue canvas_commands;

    JsonValue background;
    background["CommandType"] = "SetBackgroundStyle";
    background["Value"] = this->m_background_color.to_html_hex();
    canvas_commands.append(background);

    if(!this->m_media_id.empty())
    {
        JsonValue media;
        media["CommandType"] = "SetMedia";
        media["MediaId"] = this->m_media_id;
        canvas_commands.append(media);
    }

    if (this->m_layer_settings.size())
    {
        JsonValue layer_settings;
        layer_settings["CommandType"] = "SetLayerSettings";
        for (const auto &layer : this->m_layer_settings)
        {
            layer_settings["Value"][layer.first] = layer.second.to_json();
        }

        canvas_commands.append(layer_settings);
    }

    for (const auto &frame : this->m_frames)
    {
        canvas_commands.append(frame->to_json());
    }

    obj["CommandType"] = "CanvasCommands";
    obj["CanvasId"] = this->m_canvas_id;
    obj["Commands"] = canvas_commands;
    return obj;
}

const Color& Canvas2D::background_color() const
{
    return this->m_background_color;
}

Canvas2D& Canvas2D::background_color(const Color& value)
{
    this->m_background_color = value;
    return *this;
}

std::string Canvas2D::to_string() const
{
    return this->to_json().to_string();
}

double Canvas2D::width() const
{
    return this->m_width;
}

double Canvas2D::height() const
{
    return this->m_height;
}

} // namespace svt