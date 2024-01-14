#pragma once

#include <vector>

bool isCharOfList(char in_character, const std::vector<char>& in_charList)
{
  for (char character : in_charList)
  {
    if (in_character == character)
    {
      return true;
    }
  }

  return false;
}

std::vector<String> splitString(const String& in_string, const std::vector<char>& in_delimiterList)
{
  if (in_delimiterList.size() == 0)
  {
    return std::vector<String>();
  }

  std::vector<size_t> indexList;

  for (size_t index = 0; index < in_string.length(); ++index)
  {
    if (isCharOfList(in_string.charAt(index), in_delimiterList))
    {
      indexList.emplace_back(index);
    }
  }

  std::vector<String> stringList;

  size_t indexFrom = 0;

  for (size_t indexTo : indexList)
  {
    stringList.emplace_back(in_string.substring(indexFrom, indexTo));
    indexFrom = indexTo + 1;
  }

  if (indexFrom < in_string.length())
  {
    stringList.emplace_back(in_string.substring(indexFrom, in_string.length()));
  }

  return stringList;
}
