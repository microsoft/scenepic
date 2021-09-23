// ----------------------------------------------------------------------------
//
// text_panel.h -- An ScenePic TextPanel UI component.
//
// Copyright (C) 2019 Microsoft
//
// For conditions of distribution and use, see copyright notice in LICENSE
//
// ----------------------------------------------------------------------------

#ifndef _SP_TEXT_PANEL_H_
#define _SP_TEXT_PANEL_H_

#include <string>

#include "json_value.h"

namespace svt
{
/** Represents a ScenePic TextPanel UI component. */
class TextPanel
{
public:
    /** The title of the text panel. */
    TextPanel &title(const std::string &title);

    /** The title of the text panel. */
    const std::string &title() const;

    /** The text within the text panel */
    TextPanel &text(const std::string &text);

    /** The text within the text panel */
    const std::string &text() const;

    /** Sets the text inside the optional input box. */
    TextPanel &input_text(const std::string &text);

    /** Sets the text inside the optional input box. */
    const std::string &input_text() const;

    /** Return a JSON string representing the object */
    std::string to_string() const;

    /** Convert this object into ScenePic json.
     *  \return a json value
     */
    JsonValue to_json() const;

    /** Call this if you are dumping an ScenePic script in multiple parts,
     *  e.g. for client-server interactions, or when chunking a large
     * visualization into multiple files.
     */
    void clear_script();

private:
    friend class Scene;

    /** Constructor */
    TextPanel(const std::string &text_panel_id);

    std::string m_text_panel_id;
    std::string m_text;
    std::string m_title;
    std::string m_input_text;
};
} // namespace svt

#endif