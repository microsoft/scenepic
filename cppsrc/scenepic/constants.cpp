// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "matrix.h"

namespace scenepic
{
  const VectorBuffer& VectorBufferNone()
  {
    static VectorBuffer NONE = VectorBuffer::Zero(0, 3);
    return NONE;
  }

  const ColorBuffer& ColorBufferNone()
  {
    static ColorBuffer NONE = ColorBuffer::Zero(0, 3);
    return NONE;
  }

  const UVBuffer& UVBufferNone()
  {
    static UVBuffer NONE = UVBuffer::Zero(0, 2);
    return NONE;
  }

  const QuaternionBuffer& QuaternionBufferNone()
  {
    static QuaternionBuffer NONE = QuaternionBuffer::Zero(0, 4);
    return NONE;
  }

  const Vector& VectorNone()
  {
    const Vector::Scalar val =
      -std::numeric_limits<typename scenepic::Vector::Scalar>::infinity();
    static Vector NONE = Vector(val, val, val);
    return NONE;
  }
}