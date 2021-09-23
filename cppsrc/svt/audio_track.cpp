#include <fstream>
#include <algorithm>
#include <cctype>
#include <exception>

#include "audio_track.h"
#include "base64.h"
#include "util.h"


namespace svt
{
AudioTrack::AudioTrack(const std::string &audio_id) : m_audio_id(audio_id),
                                                      m_data()
{
}

void AudioTrack::load(const std::string &path)
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

JsonValue AudioTrack::to_json() const
{
    JsonValue obj;
    obj["CommandType"] = "DefineAudioTrack";
    obj["AudioId"] = this->m_audio_id;
    obj["Type"] = this->m_ext;
    obj["Data"] = base64_encode(this->m_data.data(),
                                static_cast<unsigned int>(this->m_data.size()));

    return obj;
}

const std::string &AudioTrack::audio_id() const
{
    return this->m_audio_id;
}

const std::vector<std::uint8_t> &AudioTrack::data() const
{
    return this->m_data;
}

std::vector<std::uint8_t> &AudioTrack::data()
{
    return this->m_data;
}

AudioTrack &AudioTrack::data(const std::vector<std::uint8_t> &value)
{
    this->m_data = value;
    return *this;
}

std::string AudioTrack::to_string() const
{
    return this->to_json().to_string();
}

} // namespace svt