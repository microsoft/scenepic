#include <cmath>

#include "focus_point.h"
#include "util.h"

namespace svt
{
FocusPoint::FocusPoint(const Vector &position,
                       const Vector &orientation_axis_angle) : m_position(position),
                                                               m_orientation_axis_angle(orientation_axis_angle)
{
}

const Vector &FocusPoint::position() const
{
    return this->m_position;
}

const Vector &FocusPoint::orientation_axis_angle() const
{
    return this->m_orientation_axis_angle;
}

JsonValue FocusPoint::to_json() const
{
    JsonValue obj;
    obj["CommandType"] = "SetFocusPoint";
    obj["Position"] = matrix_to_json(this->m_position);
    if (this->m_orientation_axis_angle != VectorNone())
    {
        obj["OrientationAxisAngle"] = matrix_to_json(this->m_orientation_axis_angle);
    }

    return obj;
}

bool FocusPoint::is_none() const
{
    return !(this->m_position != VectorNone() ||
             this->m_orientation_axis_angle != VectorNone());
}

FocusPoint &FocusPoint::operator=(const Vector &position)
{
    this->m_position = position;
    this->m_orientation_axis_angle = VectorNone();
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

} // namespace svt