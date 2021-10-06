// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "drop_down_menu.h"

#include "util.h"

namespace scenepic
{
  DropDownMenu::DropDownMenu(const std::string& drop_down_menu_id)
  : m_drop_down_menu_id(drop_down_menu_id), m_selection(0)
  {}

  DropDownMenu& DropDownMenu::title(const std::string& title)
  {
    m_title = title;
    return *this;
  }

  const std::string& DropDownMenu::title() const
  {
    return m_title;
  }

  DropDownMenu& DropDownMenu::items(const std::vector<std::string>& items)
  {
    m_items = items;
    return *this;
  }

  const std::vector<std::string>& DropDownMenu::items() const
  {
    return m_items;
  }

  std::vector<std::string>& DropDownMenu::items()
  {
    return m_items;
  }

  DropDownMenu& DropDownMenu::selection(int index)
  {
    m_selection = index;
    return *this;
  }

  int DropDownMenu::selection() const
  {
    return m_selection;
  }

  void DropDownMenu::set_disabled(int index, bool disabled)
  {
    auto pos = m_disabled_indices.find(index);
    if (disabled && pos == m_disabled_indices.end())
    {
      m_disabled_indices.insert(index);
    }

    if (!disabled && pos != m_disabled_indices.end())
    {
      m_disabled_indices.erase(pos);
    }
  }

  JsonValue DropDownMenu::to_json() const
  {
    JsonValue commands;

    if (!m_title.empty())
    {
      JsonValue command;
      command["CommandType"] = "SetDropDownMenuTitle";
      command["DropDownMenuId"] = m_drop_down_menu_id;
      command["Value"] = m_title;
      commands.append(command);
    }

    if (m_items.size())
    {
      JsonValue command;
      command["CommandType"] = "SetDropDownMenuItems";
      command["DropDownMenuId"] = m_drop_down_menu_id;
      for (const auto& item : m_items)
      {
        command["Items"].append(item);
      }
      commands.append(command);
    }

    {
      JsonValue command;
      command["CommandType"] = "SetDropDownMenuSelection";
      command["DropDownMenuId"] = m_drop_down_menu_id;
      command["Index"] = static_cast<std::int64_t>(m_selection);
      commands.append(command);
    }

    for (auto& index : m_disabled_indices)
    {
      JsonValue command;
      command["CommandType"] = "SetDropDownMenuItemDisabled";
      command["DropDownMenuId"] = m_drop_down_menu_id;
      command["Index"] = static_cast<std::int64_t>(index);
      command["Disable"] = true;
      commands.append(command);
    }

    return commands;
  }

  void DropDownMenu::clear_script()
  {
    m_title.clear();
    m_items.clear();
    m_disabled_indices.clear();
  }

  std::string DropDownMenu::to_string() const
  {
    return this->to_json().to_string();
  }

} // namespace scenepic