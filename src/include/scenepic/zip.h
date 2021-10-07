// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#ifndef _SCENEPIC_ZIP_H_
#define _SCENEPIC_ZIP_H_

#include <cstdint>
#include <vector>

namespace scenepic
{
  /** Deflate a byte array.
   *  \param data a pointer to the bytes to deflate
   *  \param data_length the number of bytes to deflate
   *  \return the deflated bytes
   */
  std::vector<std::uint8_t>
  deflate(const std::uint8_t* data, std::size_t data_length);

  /** Inflates a byte array created by @see deflate.
   *  \param data the deflated bytes
   *  \param source_length the number of deflated bytes
   *  \param dest_length an estimate of the number of original bytes
   *  \return the original bytes
   */
  std::vector<std::uint8_t> inflate(
    const std::uint8_t* data,
    std::size_t source_length,
    std::size_t dest_length);
} // namespace scenepic

#endif