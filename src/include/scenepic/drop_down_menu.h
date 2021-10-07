// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#ifndef _SCENEPIC_DROP_DOWN_MENU_H_
#define _SCENEPIC_DROP_DOWN_MENU_H_

#include "json_value.h"

#include <set>
#include <string>
#include <vector>

namespace scenepic
{
  /** Represents a ScenePic DropDownMenu UI component. */
  class DropDownMenu
  {
  public:
    /** The title of the menu. */
    DropDownMenu& title(const std::string& title);

    /** The title of the menu. */
    const std::string& title() const;

    /** The option items displayed in the drop down. */
    DropDownMenu& items(const std::vector<std::string>& items);

    /** The option items displayed in the drop down. */
    const std::vector<std::string>& items() const;

    /** The option items displayed in the drop down. */
    std::vector<std::string>& items();

    /** The current selection index. */
    DropDownMenu& selection(int index);

    /** The current selection index. */
    int selection() const;

    /** Flip a bit determining whether an index is disabled. */
    void set_disabled(int index, bool disabled);

    /** Return a JSON string representing the object */
    std::string to_string() const;

    /** Convert this object into ScenePic json.
     *  \return a json value
     */
    JsonValue to_json() const;

    /** Call this if you are dumping a ScenePic script in multiple parts,
     *  e.g. for client-server interactions, or when chunking a large
     *  visualization into multiple files.
     */
    void clear_script();

  private:
    friend class Scene;

    /** Constructor.
     *  \param drop_down_menu_id unique identifier for the dropdown menu
     */
    DropDownMenu(const std::string& drop_down_menu_id);

    std::string m_drop_down_menu_id;
    std::string m_title;
    std::vector<std::string> m_items;
    std::set<int> m_disabled_indices;
    int m_selection;
  };
} // namespace scenepic

#endif