#include <fstream>
#include <algorithm>
#include <cctype>
#include <exception>

#include "video.h"
#include "base64.h"
#include "util.h"


namespace svt
{
Video::Video(const std::string &video_id) : m_video_id(video_id),
                                                      m_data()
{
}

void Video::load(const std::string &path)
{
    std::ifstream ifs(path, std::ios::binary | std::ios::ate);
    std::ifstream::pos_type pos = ifs.tellg();

    this->m_data = std::vector<unsigned char>(static_cast<std::size_t>(pos));

    ifs.seekg(0, std::ios::beg);
    ifs.read(reinterpret_cast<char *>(this->m_data.data()), pos);

    auto idx = path.rfind('.');
    if(idx != std::string::npos){
        this->m_ext = path.substr(idx + 1);
    }else{
        throw std::invalid_argument("Unable to determine extension from path.");
    }
}

JsonValue Video::to_json() const
{
    JsonValue obj;
    obj["CommandType"] = "DefineVideo";
    obj["VideoId"] = this->m_video_id;
    obj["Type"] = this->m_ext;
    obj["Data"] = base64_encode(this->m_data.data(),
                                static_cast<unsigned int>(this->m_data.size()));

    return obj;
}

const std::string &Video::video_id() const
{
    return this->m_video_id;
}

const std::vector<std::uint8_t> &Video::data() const
{
    return this->m_data;
}

std::vector<std::uint8_t> &Video::data()
{
    return this->m_data;
}

Video &Video::data(const std::vector<std::uint8_t> &value)
{
    this->m_data = value;
    return *this;
}

std::string Video::to_string() const
{
    return this->to_json().to_string();
}

} // namespace svt