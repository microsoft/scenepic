// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#ifndef _SCENEPIC_AUDIO_TRACK_H_
#define _SCENEPIC_AUDIO_TRACK_H_

#include "json_value.h"

#include <cstdint>
#include <string>
#include <vector>

namespace scenepic
{
  /** Audio track that can be connected to a canvas. */
  class AudioTrack
  {
  public:
    /** Load an audio file from the disk
     *  \param path the path to the audio file
     */
    void load(const std::string& path);

    /** Return a JSON string representing the object */
    std::string to_string() const;

    /** Convert this object into ScenePic json.
     *  \return a json value
     */
    JsonValue to_json() const;

    /** A unique identifier for the audio */
    const std::string& audio_id() const;

    /** The encoded binary audio data */
    const std::vector<std::uint8_t>& data() const;

    /** The encoded binary audio data */
    std::vector<std::uint8_t>& data();

    /** The encoded binary audio data */
    AudioTrack& data(const std::vector<std::uint8_t>& value);

    /** The extension of the audio track (e.g. MP3, OGG) */
    const std::string& ext() const;

    /** The extension of the audio track (e.g. MP3, OGG) */
    AudioTrack& ext(const std::string& value);

  private:
    friend class Scene;

    /** Constructor
     * \param audio_id a unique identifier for the audio track
     */
    AudioTrack(const std::string& audio_id);

    std::vector<std::uint8_t> m_data;
    std::string m_audio_id;
    std::string m_ext;
  };
} // namespace scenepic

#endif