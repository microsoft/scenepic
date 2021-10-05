// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#ifndef _SCENEPIC_JSON_VALUE_H_
#define _SCENEPIC_JSON_VALUE_H_

#include <cstdint>
#include <iostream>
#include <map>
#include <string>
#include <vector>

namespace scenepic
{
  /** Types of JsonValue objects */
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

  /** Representation of a JSON value according to the specification at
   * https://www.json.org/json-en.html. */
  class JsonValue
  {
  public:
    /** Constructor. Creates a default JSON object type. */
    JsonValue();

    /** Constructor.
     *  \param type desired type of Json object
     */
    JsonValue(JsonType type);

    /** Constructor.
     *  \param value string value to represent
     */
    JsonValue(const std::string& value);

    /** Constructor.
     *  \param value string value to represent
     */
    JsonValue(std::string&& value);

    /** Constructor.
     *  \param value string value to represent
     */
    JsonValue(const char* value);

    /** Constructor.
     *  \param value floating point value to represent
     */
    JsonValue(double value);

    /** Constructor.
     *  \param value integer value to represent
     */
    JsonValue(std::int64_t value);

    /** Constructor.
     *  \param value boolean value to represent
     */
    JsonValue(bool value);

    /** Convert this objec to an array of the specified size.
     *
     *  \param size the number of elements in the array
     */
    void resize(std::size_t size);

    /** Append the object to this array.
     *
     *  \param object the object to append.
     */
    void append(JsonValue&& object);

    /** Append the object to this array.
     *
     *  \param object the object to append.
     */
    void append(const JsonValue& object);

    /** Return the value stored in this object at the specified key.
     *  \param key the locator key
     *  \return the stored object
     */
    JsonValue& operator[](const std::string& key);

    /** Return the value stored in this object at the specified key.
     *  \param key the locator key
     *  \return the stored object
     */
    const JsonValue& operator[](const std::string& key) const;

    /** Set the value of this object to the specified value.
     *  \param value the value to use
     */
    JsonValue& operator=(const std::string& value);

    /** Set the value of this object to the specified value.
     *  \param value the value to use
     */
    JsonValue& operator=(std::string&& value);

    /** Set the value of this object to the specified value.
     *  \param value the value to use
     */
    JsonValue& operator=(const char* value);

    /** Set the value of this object to the specified value.
     *  \param value the value to use
     */
    JsonValue& operator=(double value);

    /** Set the value of this object to the specified value.
     *  \param value the value to use
     */
    JsonValue& operator=(std::int64_t value);

    /** Set the value of this object to the specified value.
     *  \param value the value to use
     */
    JsonValue& operator=(bool value);

    /** The type of this object */
    JsonType type() const;

    /** A string representation of this object in valid JSON */
    std::string to_string() const;

    /** Return this object interpreted as a string. */
    const std::string& as_string() const;

    /** Return this object interpreted as a double. */
    double as_double() const;

    /** Return this object intepreted as a float. */
    float as_float() const;

    /** Return this object intepreted as an integer. */
    std::int64_t as_int() const;

    /** Return this object intepreted as a boolean. */
    bool as_boolean() const;

    /** Return the values of this JSON array. */
    const std::vector<JsonValue>& values() const;

    /** Return the key/value lookup for this JSON object. */
    const std::map<std::string, JsonValue>& lookup() const;

    /** Parse a JsonValue object from the provided input stream.
     *  \param stream an input stream
     *  \return the stream interpreted as a JSON object
     */
    static JsonValue parse(std::istream& stream);

    /** Return a singleton representating a JSON NULL object */
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