#include <fstream>
#include <algorithm>
#include <cctype>
#include <exception>

#include "image.h"
#include "base64.h"
#include "util.h"

namespace
{
bool end_with(const std::string &value, const std::string &ending)
{
    if (value.size() < ending.size())
    {
        return false;
    }

    if (value.compare(value.size() - ending.size(), ending.size(), ending) == 0)
    {
        return true;
    }

    return false;
}
} // namespace

namespace scenepic
{
Image::Image(const std::string &image_id) : m_image_id(image_id),
                                            m_data(),
                                            m_ext("None")
{
}

void Image::load(const std::string &path)
{
    std::ifstream ifs(path, std::ios::binary | std::ios::ate);
    std::ifstream::pos_type pos = ifs.tellg();

    this->m_data = std::vector<unsigned char>(static_cast<std::size_t>(pos));

    ifs.seekg(0, std::ios::beg);
    ifs.read(reinterpret_cast<char *>(this->m_data.data()), pos);

    std::string name = path;
    std::transform(name.begin(), name.end(), name.begin(),
                   [](unsigned char c) { return std::tolower(c); });

    if (end_with(name, "png"))
    {
        this->m_ext = "png";
    }
    else if (end_with(name, "jpeg") || end_with(name, "jpg"))
    {
        this->m_ext = "jpg";
    }
    else
    {
        throw std::invalid_argument("Not a path to a JPG or PNG image");
    }
}

JsonValue Image::to_json() const
{
    JsonValue obj;
    obj["CommandType"] = "DefineImage";
    obj["ImageId"] = this->m_image_id;
    obj["Type"] = this->m_ext;
    obj["Data"] = base64_encode(this->m_data.data(),
                                static_cast<unsigned int>(this->m_data.size()));

    return obj;
}

const std::string &Image::image_id() const
{
    return this->m_image_id;
}

const std::vector<std::uint8_t> &Image::data() const
{
    return this->m_data;
}

std::vector<std::uint8_t> &Image::data()
{
    return this->m_data;
}

Image &Image::data(const std::vector<std::uint8_t> &value)
{
    this->m_data = value;
    return *this;
}

const std::string &Image::ext() const
{
    return this->m_ext;
}

Image &Image::ext(const std::string &value)
{
    this->m_ext = value;
    return *this;
}

std::string Image::to_string() const
{
    return this->to_json().to_string();
}

} // namespace scenepic