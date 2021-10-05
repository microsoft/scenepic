// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#ifndef _SCENEPIC_COMPRESSION_H_
#define _SCENEPIC_COMPRESSION_H_

#include "matrix.h"
#include "zip.h"

#include <cstdint>
#include <vector>

namespace scenepic
{
  /** Compress a matrix.
   *  \param matrix the matrix to be compressed
   *  \return the compressed bytes
   */
  template<typename Derived>
  std::vector<std::uint8_t> compress_matrix(const Derived& matrix)
  {
    const std::uint8_t* source_buf =
      reinterpret_cast<const std::uint8_t*>(matrix.data());
    auto dest_len = sizeof(typename Derived::Scalar) * matrix.size();
    auto deflate_bytes = deflate(source_buf, dest_len);
    dest_len = deflate_bytes.size();
    deflate_bytes.resize(deflate_bytes.size() + 5);
    std::uint32_t* rows_ptr =
      reinterpret_cast<std::uint32_t*>(deflate_bytes.data() + dest_len);
    *rows_ptr = static_cast<std::uint32_t>(matrix.rows());
    deflate_bytes[dest_len + 4] = static_cast<std::uint8_t>(matrix.cols());

    return deflate_bytes;
  }

  /** Decompress a matrix compressed by the "compress" method.
   *  \param buffer the compressed bytes
   *  \return the decompressed vertex buffer
   */
  template<typename Derived>
  Derived decompress_matrix(const std::vector<std::uint8_t>& buffer)
  {
    const std::uint32_t* rows_ptr =
      reinterpret_cast<const std::uint32_t*>(buffer.data() + buffer.size() - 5);
    Eigen::Index rows = *rows_ptr;
    Eigen::Index cols = *buffer.rbegin();
    auto inflate_bytes = inflate(
      buffer.data(),
      buffer.size() - 8,
      rows * cols * sizeof(typename Derived::Scalar));
    Eigen::Map<Derived> matrix(
      reinterpret_cast<typename Derived::Scalar*>(inflate_bytes.data()),
      rows,
      cols);

    return matrix;
  }
} // namespace scenepic

#endif