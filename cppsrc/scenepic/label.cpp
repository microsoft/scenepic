#include <map>
#include <exception>

#include "label.h"
#include "util.h"

namespace
{
std::map<std::string, int> HORIZONTAL_ALIGNMENTS = {
    {"left", -1},
    {"center", 0},
    {"right", +1}};

std::map<std::string, int> VERTICAL_ALIGNMENTS = {
    {"top", -1},
    {"middle", 0},
    {"bottom", +1}};
} // namespace

namespace scenepic
{
Label::Label(const std::string &label_id,
             const std::string &mesh_id) : m_label_id(label_id),
                                           m_mesh_id(mesh_id),
                                           m_text("Text"),
                                           m_fill_color(Colors::White),
                                           m_size_in_pixels(12.0),
                                           m_horizontal_align("left"),
                                           m_vertical_align("middle"),
                                           m_offset_distance(0.1),
                                           m_font_family("sans-serif")
{
}

JsonValue Label::to_json() const
{
    JsonValue obj;

    obj["CommandType"] = "DefineLabel";
    obj["LabelId"] = this->m_label_id;
    obj["Text"] = this->m_text;
    obj["FillStyle"] = this->m_fill_color.to_html_hex();
    obj["FontFamily"] = this->m_font_family;
    obj["FontSizePx"] = this->m_size_in_pixels;
    obj["HorizontalAlign"] = static_cast<std::int64_t>(HORIZONTAL_ALIGNMENTS[this->m_horizontal_align]);
    obj["VerticalAlign"] = static_cast<std::int64_t>(VERTICAL_ALIGNMENTS[this->m_vertical_align]);
    obj["OffsetDistance"] = this->m_offset_distance;

    return obj;
}

const std::string &Label::label_id() const
{
    return this->m_label_id;
}

const std::string &Label::mesh_id() const
{
    return this->m_mesh_id;
}

const std::string &Label::text() const
{
    return this->m_text;
}

Label &Label::text(const std::string &value)
{
    this->m_text = value;
    return *this;
}

const Color &Label::fill_color() const
{
    return this->m_fill_color;
}

Label &Label::fill_color(const Color &value)
{
    this->m_fill_color = value;
    return *this;
}

double Label::size_in_pixels() const
{
    return this->m_size_in_pixels;
}

Label &Label::size_in_pixels(double value)
{
    this->m_size_in_pixels = value;
    return *this;
}

const std::string &Label::font_family() const
{
    return this->m_font_family;
}

Label &Label::font_family(const std::string &value)
{
    this->m_font_family = value;
    return *this;
}

const std::string &Label::horizontal_align() const
{
    return this->m_horizontal_align;
}

Label &Label::horizontal_align(const std::string &value)
{
    if (HORIZONTAL_ALIGNMENTS.find(value) == HORIZONTAL_ALIGNMENTS.end())
    {
        throw std::invalid_argument("Unsupported alignment: " + value);
    }

    this->m_horizontal_align = value;
    return *this;
}

const std::string &Label::vertical_align() const
{
    return this->m_vertical_align;
}

Label &Label::vertical_align(const std::string &value)
{
    if (VERTICAL_ALIGNMENTS.find(value) == VERTICAL_ALIGNMENTS.end())
    {
        throw std::invalid_argument("Unsupported alignment: " + value);
    }

    this->m_vertical_align = value;
    return *this;
}

double Label::offset_distance() const
{
    return this->m_offset_distance;
}

Label &Label::offset_distance(double value)
{
    this->m_offset_distance = value;
    return *this;
}

std::string Label::to_string() const
{
    return this->to_json().to_string();
}

} // namespace scenepic