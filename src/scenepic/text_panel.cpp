// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "text_panel.h"

#include "util.h"

namespace scenepic
{
  TextPanel::TextPanel(const std::string& text_panel_id)
  : m_text_panel_id(text_panel_id), m_text(""), m_title(""), m_input_text("")
  {}

  TextPanel& TextPanel::title(const std::string& title)
  {
    m_title = title;
    return *this;
  }

  const std::string& TextPanel::title() const
  {
    return m_title;
  }

  TextPanel& TextPanel::text(const std::string& text)
  {
    m_text = text;
    return *this;
  }

  const std::string& TextPanel::text() const
  {
    return m_text;
  }

  TextPanel& TextPanel::input_text(const std::string& text)
  {
    m_input_text = text;
    return *this;
  }

  const std::string& TextPanel::input_text() const
  {
    return m_input_text;
  }

  JsonValue TextPanel::to_json() const
  {
    JsonValue commands;
    if (!m_text.empty())
    {
      JsonValue command;
      command["CommandType"] = "SetTextPanelValue";
      command["TextPanelId"] = m_text_panel_id;
      command["Value"] = m_text;
      commands.append(command);
    }

    if (!m_title.empty())
    {
      JsonValue command;
      command["CommandType"] = "SetTextPanelTitle";
      command["TextPanelId"] = m_text_panel_id;
      command["Value"] = m_title;
      commands.append(command);
    }

    if (!m_input_text.empty())
    {
      JsonValue command;
      command["CommandType"] = "SetTextPanelInputText";
      command["TextPanelId"] = m_text_panel_id;
      command["Value"] = m_input_text;
      commands.append(command);
    }

    return commands;
  }

  void TextPanel::clear_script()
  {
    m_text.clear();
    m_title.clear();
    m_input_text.clear();
  }

  std::string TextPanel::to_string() const
  {
    return this->to_json().to_string();
  }
} // namespace scenepic