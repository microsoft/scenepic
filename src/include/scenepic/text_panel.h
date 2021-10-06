// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#ifndef _SCENEPIC_TEXT_PANEL_H_
#define _SCENEPIC_TEXT_PANEL_H_

#include "json_value.h"

#include <string>

namespace scenepic
{
  /** Represents a ScenePic TextPanel UI component. */
  class TextPanel
  {
  public:
    /** The title of the text panel. */
    TextPanel& title(const std::string& title);

    /** The title of the text panel. */
    const std::string& title() const;

    /** The text within the text panel */
    TextPanel& text(const std::string& text);

    /** The text within the text panel */
    const std::string& text() const;

    /** Sets the text inside the optional input box. */
    TextPanel& input_text(const std::string& text);

    /** Sets the text inside the optional input box. */
    const std::string& input_text() const;

    /** Return a JSON string representing the object */
    std::string to_string() const;

    /** Convert this object into ScenePic json.
     *  \return a json value
     */
    JsonValue to_json() const;

    /** Call this if you are dumping a ScenePic script in multiple parts,
     *  e.g. for client-server interactions, or when chunking a large
     * visualization into multiple files.
     */
    void clear_script();

  private:
    friend class Scene;

    /** Constructor */
    TextPanel(const std::string& text_panel_id);

    std::string m_text_panel_id;
    std::string m_text;
    std::string m_title;
    std::string m_input_text;
  };
} // namespace scenepic

#endif