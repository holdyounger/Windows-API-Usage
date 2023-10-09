#ifndef COMMON_BASE_HELPER_WIN_STR_UTILS_H_
#define COMMON_BASE_HELPER_WIN_STR_UTILS_H_

#include <string>

#ifdef _WIN32
namespace StrUtils {
std::string UnicodeToUTF8(const wchar_t* str, int strLen);
std::string UnicodeToAnsi(const wchar_t* str, int strLen);
std::wstring AnsiToUnicode(const char* str, int strLen);
std::string AnsiToUtf8(const std::string& str);
std::wstring Utf8ToUnicode(const std::string& str);
}  // namespace ztnaclient
#endif

#endif  //COMMON_BASE_HELPER_WIN_STR_UTILS_H_