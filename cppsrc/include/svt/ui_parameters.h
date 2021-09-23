// ----------------------------------------------------------------------------
//
// ui_parameters.h -- Per-Canvas3D parameters of the ScenePic user interface
//
// Copyright (C) 2019 Microsoft
//
// For conditions of distribution and use, see copyright notice in LICENSE
//
// ----------------------------------------------------------------------------

#ifndef _SP_UI_PARAMETERS_H_
#define _SP_UI_PARAMETERS_H_

#include "json_value.h"

namespace svt
{
/** Per-Canvas3D parameters of the ScenePic user interface */
class UIParameters
{
public:
    /** Constructor.
     *  \param pointer_alt_key_multiplier when the Alt key is pressed, mouse movement is scaled by this factor.
     *  \param pointer_rotation_speed linear scale factor of pointer translation into camera rotation.
     *  \param mouse_wheel_translation_speed linear scale factor of mouse wheel input into camera translation.
     */
    UIParameters(double pointer_alt_key_multiplier = 0.2,
                 double pointer_rotation_speed = 0.01,
                 double mouse_wheel_translation_speed = 0.005);

    /** When the Alt key is pressed, mouse movement is scaled by this factor. */
    double pointer_alt_key_multiplier() const;

    /** Linear scale factor of pointer translation into camera rotation. */
    double pointer_rotation_speed() const;

    /** Linear scale factor of mouse wheel input into camera translation. */
    double mouse_wheel_translation_speed() const;

    /** Return a JSON string representing the object */
    std::string to_string() const;

    /** Convert this object into ScenePic json.
     *  \return a json value
     */
    JsonValue to_json() const;

    /** Whether this is a "no ui parameters" instance */
    bool is_none() const;

    /** A UIParameters value indicating "no ui parameters" */
    static const UIParameters None();

private:
    double m_pointer_alt_key_multiplier;
    double m_pointer_rotation_speed;
    double m_mouse_wheel_translation_speed;
};
} // namespace svt

#endif