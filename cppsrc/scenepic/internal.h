// ----------------------------------------------------------------------------
//
// internal.h -- Functions only used internally
//
// Copyright (C) 2020 Microsoft
//
// For conditions of distribution and use, see copyright notice in LICENSE
//
// ----------------------------------------------------------------------------

#include "json/json.h"
#include "json_value.h"

namespace scenepic
{
    Json::Value scenepic_to_json(const JsonValue &value);
}