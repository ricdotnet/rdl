#pragma once

#include <string>
#include "../interpreter.hpp"

inline std::string normalise_identifier(std::string identifier)
{
  if (identifier.front() == '$')
  {
    identifier.erase(0, 1);
  }

  return identifier;
}

inline std::vector<Value> split(const std::string &string, const std::string &delim)
{
  std::vector<Value> elements;

  if (delim.empty())
  {
    for (size_t i = 0; i < string.length(); i++)
    {
      elements.push_back(Value::string_value(string.substr(i, 1)));
    }
    return elements;
  }

  size_t pos = 0;
  size_t start = 0;

  while ((pos = string.find(delim, start)) != std::string::npos)
  {
    elements.push_back(Value::string_value(string.substr(start, pos - start)));
    start = pos + delim.length();
  }

  elements.push_back(Value::string_value(string.substr(start)));

  return elements;
}

inline std::string to_upper(const std::string &s)
{
  std::string result;
  std::ranges::transform(s, std::back_inserter(result), ::toupper);
  return result;
}

inline std::string to_lower(const std::string &s)
{
  std::string result;
  std::ranges::transform(s, std::back_inserter(result), ::tolower);
  return result;
}
