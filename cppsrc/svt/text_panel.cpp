#include "text_panel.h"
#include "util.h"

namespace svt
{
TextPanel::TextPanel(const std::string &text_panel_id) : m_text_panel_id(text_panel_id), m_text(""), m_title(""), m_input_text("")
{
}

TextPanel &TextPanel::title(const std::string &title)
{
    this->m_title = title;
    return *this;
}

const std::string &TextPanel::title() const
{
    return this->m_title;
}

TextPanel &TextPanel::text(const std::string &text)
{
    this->m_text = text;
    return *this;
}

const std::string &TextPanel::text() const
{
    return this->m_text;
}

TextPanel &TextPanel::input_text(const std::string &text)
{
    this->m_input_text = text;
    return *this;
}

const std::string &TextPanel::input_text() const
{
    return this->m_input_text;
}

JsonValue TextPanel::to_json() const
{
    JsonValue commands;
    if (!this->m_text.empty())
    {
        JsonValue command;
        command["CommandType"] = "SetTextPanelValue";
        command["TextPanelId"] = this->m_text_panel_id;
        command["Value"] = this->m_text;
        commands.append(command);
    }

    if (!this->m_title.empty())
    {
        JsonValue command;
        command["CommandType"] = "SetTextPanelTitle";
        command["TextPanelId"] = this->m_text_panel_id;
        command["Value"] = this->m_title;
        commands.append(command);
    }

    if (!this->m_input_text.empty())
    {
        JsonValue command;
        command["CommandType"] = "SetTextPanelInputText";
        command["TextPanelId"] = this->m_text_panel_id;
        command["Value"] = this->m_input_text;
        commands.append(command);
    }

    return commands;
}

void TextPanel::clear_script()
{
    this->m_text.clear();
    this->m_title.clear();
    this->m_input_text.clear();
}

std::string TextPanel::to_string() const
{
    return this->to_json().to_string();
}
} // namespace svt