// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "focus_point.h"

#include "util.h"

#include <cmath>

namespace scenepic
{
  FocusPoint::FocusPoint(
    const Vector& position, const Vector& orientation_axis_angle)
  : m_position(position), m_orientation_axis_angle(orientation_axis_angle)
  {}

  const Vector& FocusPoint::position() const
  {
    return m_position;
  }

  const Vector& FocusPoint::orientation_axis_angle() const
  {
    return m_orientation_axis_angle;
  }

  JsonValue FocusPoint::to_json() const
  {
    JsonValue obj;
    obj["CommandType"] = "SetFocusPoint";
    obj["Position"] = matrix_to_json(m_position);
    if (m_orientation_axis_angle != VectorNone())
    {
      obj["OrientationAxisAngle"] = matrix_to_json(m_orientation_axis_angle);
    }

    return obj;
  }

  bool FocusPoint::is_none() const
  {
    return !(
      m_position != VectorNone() || m_orientation_axis_angle != VectorNone());
  }

  FocusPoint& FocusPoint::operator=(const Vector& position)
  {
    m_position = position;
    m_orientation_axis_angle = VectorNone();
    return *this;
  }

  FocusPoint FocusPoint::None()
  {
    return FocusPoint(VectorNone());
  }

  std::string FocusPoint::to_string() const
  {
    return this->to_json().to_string();
  }

} // namespace scenepic