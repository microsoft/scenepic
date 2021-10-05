// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "video.h"

#include "base64.h"
#include "util.h"

#include <algorithm>
#include <cctype>
#include <exception>
#include <fstream>

namespace scenepic
{
  Video::Video(const std::string& video_id) : m_video_id(video_id), m_data() {}

  void Video::load(const std::string& path)
  {
    std::ifstream ifs(path, std::ios::binary | std::ios::ate);
    std::ifstream::pos_type pos = ifs.tellg();

    m_data = std::vector<unsigned char>(static_cast<std::size_t>(pos));

    ifs.seekg(0, std::ios::beg);
    ifs.read(reinterpret_cast<char*>(m_data.data()), pos);

    auto idx = path.rfind('.');
    if (idx != std::string::npos)
    {
      m_ext = path.substr(idx + 1);
    }
    else
    {
      throw std::invalid_argument("Unable to determine extension from path.");
    }
  }

  JsonValue Video::to_json() const
  {
    JsonValue obj;
    obj["CommandType"] = "DefineVideo";
    obj["VideoId"] = m_video_id;
    obj["Type"] = m_ext;
    obj["Data"] =
      base64_encode(m_data.data(), static_cast<unsigned int>(m_data.size()));

    return obj;
  }

  const std::string& Video::video_id() const
  {
    return m_video_id;
  }

  const std::vector<std::uint8_t>& Video::data() const
  {
    return m_data;
  }

  std::vector<std::uint8_t>& Video::data()
  {
    return m_data;
  }

  Video& Video::data(const std::vector<std::uint8_t>& value)
  {
    m_data = value;
    return *this;
  }

  std::string Video::to_string() const
  {
    return this->to_json().to_string();
  }

} // namespace scenepic