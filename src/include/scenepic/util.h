// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#ifndef _SCENEPIC_UTIL_H_
#define _SCENEPIC_UTIL_H_

#include <map>
#include <sstream>
#include <string>
#include <vector>

namespace scenepic
{
  template<typename T>
  void split(const std::string& s, char delim, T result)
  {
    std::istringstream iss(s);
    std::string item;
    while (std::getline(iss, item, delim))
    {
      if (!item.empty())
      {
        *result++ = item;
      }
    }
  }

  template<typename T>
  void unique_index(
    const std::vector<T>& items,
    std::vector<T>& unique_items,
    std::vector<std::size_t>& reverse_index)
  {
    if (items.size() == 0)
    {
      return;
    }

    std::map<T, std::size_t> lookup;
    for (auto& item : items)
    {
      if (lookup.count(item))
      {
        reverse_index.push_back(lookup[item]);
      }
      else
      {
        std::size_t index = unique_items.size();
        lookup[item] = index;
        unique_items.push_back(item);
        reverse_index.push_back(index);
      }
    }
  }
} // namespace scenepic

#endif