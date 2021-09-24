#include <limits>

#include "ui_parameters.h"
#include "util.h"

namespace scenepic
{
UIParameters::UIParameters(double pointer_alt_key_multiplier,
                           double pointer_rotation_speed,
                           double mouse_wheel_translation_speed) : m_pointer_alt_key_multiplier(pointer_alt_key_multiplier),
                                                                   m_pointer_rotation_speed(pointer_rotation_speed),
                                                                   m_mouse_wheel_translation_speed(mouse_wheel_translation_speed)
{
}

double UIParameters::pointer_alt_key_multiplier() const
{
    return m_pointer_alt_key_multiplier;
}

double UIParameters::pointer_rotation_speed() const
{
    return m_pointer_rotation_speed;
}

double UIParameters::mouse_wheel_translation_speed() const
{
    return m_mouse_wheel_translation_speed;
}

JsonValue UIParameters::to_json() const
{
    JsonValue obj;
    obj["CommandType"] = "ConfigureUserInterface";
    obj["PointerAltKeyMultiplier"] = m_pointer_alt_key_multiplier;
    obj["PointerRotationSpeed"] = m_pointer_rotation_speed;
    obj["MouseWheelTranslationSpeed"] = m_mouse_wheel_translation_speed;
    return obj;
}

bool UIParameters::is_none() const
{
    return !(m_pointer_alt_key_multiplier != -std::numeric_limits<double>::infinity() ||
             m_pointer_rotation_speed != -std::numeric_limits<double>::infinity() ||
             m_mouse_wheel_translation_speed != -std::numeric_limits<double>::infinity());
}

std::string UIParameters::to_string() const
{
    return this->to_json().to_string();
}

const UIParameters UIParameters::None()
{
    return UIParameters(-std::numeric_limits<double>::infinity(),
                        -std::numeric_limits<double>::infinity(),
                        -std::numeric_limits<double>::infinity());
}

} // namespace scenepic