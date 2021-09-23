#include "drop_down_menu.h"
#include "util.h"

namespace svt
{
DropDownMenu::DropDownMenu(const std::string &drop_down_menu_id) : m_drop_down_menu_id(drop_down_menu_id),
                                                                   m_selection(0)
{
}

DropDownMenu &DropDownMenu::title(const std::string &title)
{
    this->m_title = title;
    return *this;
}

const std::string &DropDownMenu::title() const
{
    return this->m_title;
}

DropDownMenu &DropDownMenu::items(const std::vector<std::string> &items)
{
    this->m_items = items;
    return *this;
}

const std::vector<std::string> &DropDownMenu::items() const
{
    return this->m_items;
}

std::vector<std::string> &DropDownMenu::items()
{
    return this->m_items;
}

DropDownMenu &DropDownMenu::selection(int index)
{
    this->m_selection = index;
    return *this;
}

int DropDownMenu::selection() const
{
    return this->m_selection;
}

void DropDownMenu::set_disabled(int index, bool disabled)
{
    auto pos = this->m_disabled_indices.find(index);
    if (disabled && pos == this->m_disabled_indices.end())
    {
        this->m_disabled_indices.insert(index);
    }

    if (!disabled && pos != this->m_disabled_indices.end())
    {
        this->m_disabled_indices.erase(pos);
    }
}

JsonValue DropDownMenu::to_json() const
{
    JsonValue commands;

    if (!this->m_title.empty())
    {
        JsonValue command;
        command["CommandType"] = "SetDropDownMenuTitle";
        command["DropDownMenuId"] = this->m_drop_down_menu_id;
        command["Value"] = this->m_title;
        commands.append(command);
    }

    if (this->m_items.size())
    {
        JsonValue command;
        command["CommandType"] = "SetDropDownMenuItems";
        command["DropDownMenuId"] = this->m_drop_down_menu_id;
        for (const auto &item : this->m_items)
        {
            command["Items"].append(item);
        }
        commands.append(command);
    }

    {
        JsonValue command;
        command["CommandType"] = "SetDropDownMenuSelection";
        command["DropDownMenuId"] = this->m_drop_down_menu_id;
        command["Index"] = static_cast<std::int64_t>(this->m_selection);
        commands.append(command);
    }

    for (auto &index : this->m_disabled_indices)
    {
        JsonValue command;
        command["CommandType"] = "SetDropDownMenuItemDisabled";
        command["DropDownMenuId"] = this->m_drop_down_menu_id;
        command["Index"] = static_cast<std::int64_t>(index);
        command["Disable"] = true;
        commands.append(command);
    }

    return commands;
}

void DropDownMenu::clear_script()
{
    this->m_title.clear();
    this->m_items.clear();
    this->m_disabled_indices.clear();
}

std::string DropDownMenu::to_string() const
{
    return this->to_json().to_string();
}

} // namespace svt