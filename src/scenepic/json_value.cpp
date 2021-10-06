// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "json_value.h"

#include "internal.h"

#include "json/json.h"
#include <exception>

namespace scenepic
{
  Json::Value scenepic_to_json(const JsonValue& value)
  {
    Json::Value obj;
    switch (value.type())
    {
      case JsonType::Double:
        obj = Json::Value(value.as_double());
        break;

      case JsonType::Integer:
        obj = Json::Value(value.as_int());
        break;

      case JsonType::Boolean:
        obj = Json::Value(value.as_boolean());
        break;

      case JsonType::String:
        obj = Json::Value(value.as_string());
        break;

      case JsonType::Array:
        obj.resize(0);
        for (const auto& value : value.values())
        {
          obj.append(scenepic_to_json(value));
        }

        break;

      case JsonType::Object:
        for (const auto& pair : value.lookup())
        {
          obj[pair.first] = scenepic_to_json(pair.second);
        }

        break;

      case JsonType::Null:
        return Json::Value::nullSingleton();

      default:
        throw std::runtime_error("Unsupported Json value type");
    }

    return obj;
  }

  JsonValue json_to_scenepic(const Json::Value& value)
  {
    JsonValue obj;
    switch (value.type())
    {
      case Json::ValueType::realValue:
        obj = value.asDouble();
        break;

      case Json::ValueType::intValue:
        obj = value.asInt64();
        break;

      case Json::ValueType::booleanValue:
        obj = value.asBool();
        break;

      case Json::ValueType::stringValue:
        obj = value.asString();
        break;

      case Json::ValueType::arrayValue:
        obj.resize(0);
        for (int i = 0; i < value.size(); ++i)
        {
          obj.append(json_to_scenepic(value[i]));
        }

        break;

      case Json::ValueType::objectValue:
        for (const auto& key : value.getMemberNames())
        {
          obj[key] = json_to_scenepic(value[key]);
        }

        break;

      case Json::ValueType::nullValue:
        return JsonValue::nullSingleton();

      default:
        throw std::runtime_error("Unsupported Json value type");
    }

    return obj;
  }

  std::string JsonValue::to_string() const
  {
    Json::StreamWriterBuilder builder;
    builder["commentStyle"] = "None";
    builder["indentation"] = "\t";
    builder["enableYAMLCompatibility"] = true;
    std::unique_ptr<Json::StreamWriter> writer(builder.newStreamWriter());
    std::stringstream buffer;
    writer->write(scenepic_to_json(*this), &buffer);
    buffer << std::endl;
    return buffer.str();
  }

  JsonValue JsonValue::parse(std::istream& stream)
  {
    Json::Value root;
    Json::CharReaderBuilder rbuilder;
    rbuilder["collectComments"] = false;
    std::string errs;
    if (Json::parseFromStream(rbuilder, stream, &root, &errs))
    {
      return json_to_scenepic(root);
    }

    throw std::logic_error(errs);
  }

  JsonValue::JsonValue() : JsonValue(JsonType::Object) {}

  JsonValue::JsonValue(std::int64_t value)
  : m_type(JsonType::Integer), m_int(value), m_double(value)
  {}

  JsonValue::JsonValue(double value)
  : m_type(JsonType::Double),
    m_double(value),
    m_int(static_cast<std::int64_t>(value))
  {}

  JsonValue::JsonValue(bool value) : m_type(JsonType::Boolean), m_bool(value) {}

  JsonValue::JsonValue(const std::string& value)
  : m_type(JsonType::String), m_string(value)
  {}

  JsonValue::JsonValue(const char* value)
  : m_type(JsonType::String), m_string(value)
  {}

  JsonValue::JsonValue(std::string&& value)
  : m_type(JsonType::String), m_string(std::move(value))
  {}

  JsonValue::JsonValue(JsonType type) : m_type(type) {}

  void JsonValue::resize(std::size_t size)
  {
    m_type = JsonType::Array;
    m_values.resize(size);
  }

  void JsonValue::append(const JsonValue& value)
  {
    m_type = JsonType::Array;
    m_values.push_back(value);
  }

  void JsonValue::append(JsonValue&& value)
  {
    m_type = JsonType::Array;
    m_values.push_back(std::move(value));
  }

  JsonValue& JsonValue::operator[](const std::string& key)
  {
    if (m_lookup.count(key) == 0)
    {
      m_lookup[key] = JsonValue();
    }

    m_type = JsonType::Object;
    return m_lookup[key];
  }

  const JsonValue& JsonValue::operator[](const std::string& key) const
  {
    return m_lookup.at(key);
  }

  JsonValue& JsonValue::operator=(const std::string& value)
  {
    m_type = JsonType::String;
    m_string = value;
    return *this;
  }

  JsonValue& JsonValue::operator=(const char* value)
  {
    m_type = JsonType::String;
    m_string = value;
    return *this;
  }

  JsonValue& JsonValue::operator=(std::string&& value)
  {
    m_type = JsonType::String;
    m_string = std::move(value);
    return *this;
  }

  JsonValue& JsonValue::operator=(double value)
  {
    m_type = JsonType::Double;
    m_double = value;
    m_int = static_cast<std::int64_t>(value);
    return *this;
  }

  JsonValue& JsonValue::operator=(std::int64_t value)
  {
    m_type = JsonType::Integer;
    m_int = value;
    m_double = value;
    return *this;
  }

  JsonValue& JsonValue::operator=(bool value)
  {
    m_type = JsonType::Boolean;
    m_bool = value;
    return *this;
  }

  JsonType JsonValue::type() const
  {
    return m_type;
  }

  const std::string& JsonValue::as_string() const
  {
    return m_string;
  }

  double JsonValue::as_double() const
  {
    return m_double;
  }

  float JsonValue::as_float() const
  {
    return static_cast<float>(m_double);
  }

  std::int64_t JsonValue::as_int() const
  {
    return m_int;
  }

  bool JsonValue::as_boolean() const
  {
    return m_bool;
  }

  const std::vector<JsonValue>& JsonValue::values() const
  {
    return m_values;
  }

  const std::map<std::string, JsonValue>& JsonValue::lookup() const
  {
    return m_lookup;
  }

  JsonValue JsonValue::nullSingleton()
  {
    return JsonValue(JsonType::Null);
  }
}