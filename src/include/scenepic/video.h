// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#ifndef _SCENEPIC_VIDEO_H_
#define _SCENEPIC_VIDEO_H_

#include "json_value.h"

namespace scenepic
{
  /** Video that can be connected to a canvas. */
  class Video
  {
  public:
    /** Load an video file from the disk
     *  \param path the path to the video file
     */
    void load(const std::string& path);

    /** Return a JSON string representing the object */
    std::string to_string() const;

    /** Convert this object into ScenePic json.
     *  \return a json value
     */
    JsonValue to_json() const;

    /** A unique identifier for the video */
    const std::string& video_id() const;

    /** The encoded binary video data */
    const std::vector<std::uint8_t>& data() const;

    /** The encoded binary video data */
    std::vector<std::uint8_t>& data();

    /** The encoded binary video data */
    Video& data(const std::vector<std::uint8_t>& value);

    /** The extension of the video (e.g. MP4, MKV) */
    const std::string& ext() const;

    /** The extension of the video (e.g. MP4, MKV) */
    Video& ext(const std::string& value);

  private:
    friend class Scene;

    /** Constructor
     * \param video_id a unique identifier for the audio track
     */
    Video(const std::string& video_id);

    std::vector<std::uint8_t> m_data;
    std::string m_video_id;
    std::string m_ext;
  };
} // namespace scenepic

#endif