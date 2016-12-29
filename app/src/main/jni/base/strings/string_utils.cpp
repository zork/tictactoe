////
// string_utils.cpp
////

#include "base/strings/string_utils.h"

#include "base/macros.h"

#include <vector>

namespace {

struct EscapeChars {
  char value;
  char escape_value[4];
};

const EscapeChars kEscapeChars[] = {
    {'%', "%25"},  // Percent needs to be encoded first, or bad things happen.
    {'!', "%21"}, {'*', "%2A"}, {'\'', "%27"}, {'(', "%28"}, {')', "%29"},
    {';', "%3B"}, {':', "%3A"}, {'@', "%40"},  {'&', "%26"}, {'=', "%3D"},
    {'+', "%2B"}, {'$', "%24"}, {',', "%2C"},  {'/', "%2F"}, {'?', "%3F"},
    {'#', "%23"}, {'[', "%5B"}, {']', "%5D"},
};
}

void StringReplace(char value, const char* replace, std::string* str) {
  size_t pos = 0;
  while (std::string::npos != (pos = str->find(value, pos))) {
    str->replace(pos, 1, replace);
    pos += strlen(replace);
  }
}

void PercentEscape(std::string* str) {
  for (int i = 0; i < arraysize(kEscapeChars); ++i) {
    StringReplace(kEscapeChars[i].value, kEscapeChars[i].escape_value, str);
  }
}

std::string GetFileExtension(const std::string& filename) {
  size_t pos = filename.rfind('.');
  if (pos == std::string::npos) {
    return std::string();
  }
  return filename.substr(pos + 1);
}

#if OS_WIN
bool UTF8ToWide(const char* input, std::wstring* output) {
  int size =
      MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, input, -1, NULL, 0);
  if (!size)
    return false;

  std::vector<wchar_t> data(size);
  int success = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, input, -1,
                                    &data[0], size);

  *output = &data[0];

  return success;
}

bool UTF8ToWide(const std::string& input, std::wstring* output) {
  return UTF8ToWide(input.c_str(), output);
}

bool WideToUTF8(const wchar_t* input, std::string* output) {
  int size = WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, input, -1, NULL,
                                 0, NULL, NULL);
  if (!size)
    return false;

  std::vector<char> data(size);
  int success = WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, input, -1,
                                    &data[0], size, NULL, NULL);

  *output = &data[0];

  return success;
}

bool WideToUTF8(const std::wstring& input, std::string* output) {
  return WideToUTF8(input.c_str(), output);
}
#endif
