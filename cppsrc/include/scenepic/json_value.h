// ----------------------------------------------------------------------------
//
// json_value.h -- Internal class providing JSON functionality. Acts a shim
//                 for jsoncpp, which provides Json IO internally.
//
// Copyright (C) 2021 Microsoft
//
// For conditions of distribution and use, see copyright notice in LICENSE
//
// ----------------------------------------------------------------------------

#ifndef _SCENEPIC_JSON_VALUE_H_
#define _SCENEPIC_JSON_VALUE_H_

#include <cstdint>
#include <string>
#include <map>
#include <vector>
#include <iostream>

namespace scenepic
{
    enum class JsonType
    {
        Object,
        Array,
        String,
        Integer,
        Double,
        Boolean,
        Null
    };

    class JsonValue
    {
    public:
        JsonValue();
        JsonValue(JsonType type);
        JsonValue(const std::string &value);
        JsonValue(std::string &&value);
        JsonValue(const char* value);
        JsonValue(double value);
        JsonValue(std::int64_t value);
        JsonValue(bool value);

        void resize(std::size_t size);
        void append(JsonValue &&object);
        void append(const JsonValue& object);
        JsonValue& operator[](const std::string& key);
        const JsonValue& operator[](const std::string& key) const;
        JsonValue& operator=(const std::string& value);
        JsonValue& operator=(std::string&& value);
        JsonValue& operator=(const char *value);
        JsonValue& operator=(double value);
        JsonValue& operator=(std::int64_t value);
        JsonValue& operator=(bool value);

        JsonType type() const;
        std::string to_string() const;
        const std::string& as_string() const;
        double as_double() const;
        float as_float() const;
        std::int64_t as_int() const;
        bool as_boolean() const;
        const std::vector<JsonValue>& values() const;
        const std::map<std::string, JsonValue>& lookup() const;

        static JsonValue parse(std::istream& stream);
        static JsonValue nullSingleton();

    private:
        std::string m_string;
        double m_double;
        std::int64_t m_int;
        bool m_bool;
        JsonType m_type;
        std::map<std::string, JsonValue> m_lookup;
        std::vector<JsonValue> m_values;
    };
} // namespace scenepic

#endif