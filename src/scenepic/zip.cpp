// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "zip.h"

#include "miniz/miniz.h"

namespace scenepic
{
  std::vector<std::uint8_t>
  deflate(const std::uint8_t* data, std::size_t data_length)
  {
    mz_ulong source_len = static_cast<mz_ulong>(data_length);
    mz_ulong dest_len = mz_compressBound(source_len);
    std::vector<std::uint8_t> deflate_bytes(dest_len);
    mz_compress(deflate_bytes.data(), &dest_len, data, source_len);
    deflate_bytes.resize(dest_len);
    return deflate_bytes;
  }

  std::vector<std::uint8_t> inflate(
    const std::uint8_t* data,
    std::size_t source_length,
    std::size_t dest_length)
  {
    mz_ulong dest_len = static_cast<mz_ulong>(dest_length);
    std::vector<std::uint8_t> inflate_bytes(dest_len);
    mz_ulong source_len = static_cast<mz_ulong>(source_length);
    mz_uncompress(inflate_bytes.data(), &dest_len, data, source_len);
    return inflate_bytes;
  }
}