// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#ifndef _SCENEPIC_IMAGE_H_
#define _SCENEPIC_IMAGE_H_

#include "json_value.h"

#include <cstdint>
#include <string>
#include <vector>

namespace scenepic
{
  /** A ScenePic Image type */
  class Image
  {
  public:
    /** Load an image file from the disk
     *  \param path the path to the image file
     */
    void load(const std::string& path);

    /** Return a JSON string representing the object */
    std::string to_string() const;

    /** Convert this object into ScenePic json.
     *  \return a json value
     */
    JsonValue to_json() const;

    /** A unique identifier for the image */
    const std::string& image_id() const;

    /** The encoded binary image data */
    const std::vector<std::uint8_t>& data() const;

    /** The encoded binary image data */
    std::vector<std::uint8_t>& data();

    /** The encoded binary image data */
    Image& data(const std::vector<std::uint8_t>& value);

    /** The extension of the image (e.g. JPG, PNG) */
    const std::string& ext() const;

    /** The extension of the image (e.g. JPG, PNG) */
    Image& ext(const std::string& value);

  private:
    friend class Scene;

    /** Constructor
     * \param image_id a unique identifier for the image
     */
    Image(const std::string& image_id);

    std::vector<std::uint8_t> m_data;
    std::string m_image_id;
    std::string m_ext;
  };
} // namespace scenepic

#endif