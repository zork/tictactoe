////
// string_utils.h
////

#pragma once

#include "base/platform.h"

#include <string>

void StringReplace(char value, const char* replace, std::string* str);
void PercentEscape(std::string* str);
std::string GetFileExtension(const std::string& filename);

#if OS_WIN
bool UTF8ToWide(const char* input, std::wstring* output);
bool UTF8ToWide(const std::string& input, std::wstring* output);
bool WideToUTF8(const wchar_t* input, std::string* output);
bool WideToUTF8(const std::wstring& input, std::string* output);
#endif
