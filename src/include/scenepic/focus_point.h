// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#ifndef _SCENEPIC_FOCUS_POINT_H_
#define _SCENEPIC_FOCUS_POINT_H_

#include "json_value.h"
#include "matrix.h"

namespace scenepic
{
  /** 3D focus point for this frame (with optional 3D
   *  rotation for orientation lock), used in the ScenePic user interface.
   */
  class FocusPoint
  {
  public:
    /** Constructor.
     *  \param position the position of the focus point
     *  \param orientation_axis_angle the orientation axis angle
     */
    FocusPoint(
      const Vector& position = Vector::Zero(),
      const Vector& orientation_axis_angle = VectorNone());

    /** Return a JSON string representing the object */
    std::string to_string() const;

    /** Convert this object into ScenePic json.
     *  \return a json value
     */
    JsonValue to_json() const;

    /** Whether this is a "none" focus point */
    bool is_none() const;

    /** A value indicating "no focus point" */
    static FocusPoint None();

    /** Assignment operator. */
    FocusPoint& operator=(const Vector& position);

    /** The position of the focus point */
    const Vector& position() const;

    /** The orientation axis angle */
    const Vector& orientation_axis_angle() const;

  private:
    Vector m_position;
    Vector m_orientation_axis_angle;
  };
} // namespace scenepic

#endif