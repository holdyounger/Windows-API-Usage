#include "win_str_utils.h"
#include <vector>
#ifdef _WIN32
#include <Windows.h>
#endif

#ifdef _WIN32
std::string StrUtils::UnicodeToUTF8(const wchar_t* str, int strLen) {
  std::string res;

  // 转换为utf8时，一个汉字占三个字节，需要足够的空间
  int bufSize = strLen * 3 + 1;
  CHAR* buf = new (std::nothrow) CHAR[bufSize];
  if (buf == NULL) {
    return std::string("");
  }
  memset(buf, 0, bufSize);
  WideCharToMultiByte(CP_UTF8, 0, str, strLen, buf, bufSize, NULL, NULL);
  res = buf;
  delete[] buf;
  return res;
}

std::string StrUtils::UnicodeToAnsi(const wchar_t* str, int strLen) {
  std::string res;

  // 转换为utf8时，一个汉字占三个字节，需要足够的空间
  int bufSize = strLen * 3 + 1;
  CHAR* buf = new (std::nothrow) CHAR[bufSize];
  if (buf == NULL) {
    return std::string("");
  }
  memset(buf, 0, bufSize);
  WideCharToMultiByte(CP_ACP, 0, str, strLen, buf, bufSize, NULL, NULL);
  res = buf;
  delete[] buf;
  return res;
}

std::wstring StrUtils::AnsiToUnicode(const char* str, int strLen) {
  std::wstring res;
  int bufSize = strLen + 1;
  WCHAR* buf = new (std::nothrow) WCHAR[bufSize];
  if (buf == NULL) return std::wstring(L"");

  memset(buf, 0, bufSize * sizeof(WCHAR));
  MultiByteToWideChar(CP_ACP, 0, str, (int)strLen, buf, bufSize);
  res = buf;
  delete[] buf;
  return res;
}

std::string StrUtils::AnsiToUtf8(const std::string& str) {
  std::wstring uniStr = AnsiToUnicode(str.c_str(), str.length());
  std::string res = UnicodeToUTF8(uniStr.c_str(), uniStr.length());
  return res;
}
std::wstring StrUtils::Utf8ToUnicode(const std::string& str) {
    int needChar = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, 0, 0);
    if (needChar > 0)
    {
        std::vector<wchar_t> temp(needChar);
        MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, &temp[0], needChar);
        return std::wstring(&temp[0]);
    }
    return std::wstring();
}
#endif