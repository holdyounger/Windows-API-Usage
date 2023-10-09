<h1 align="center">Welcome to win_str_utils 👋</h1>
<p>
  <a href="https://www.npmjs.com/package/" target="_blank">
    <img alt="Version" src="https://img.shields.io/badge/windows-v0.01-red">
  </a>
</p>

> windows 字符编码转换相关函数

## Author

👤 **mingming**

* Github: [@holdyounger](https://github.com/holdyounger)

## Show your support

Give a ⭐️ if this project helped you!

# Windows 字符编码转换函数

## 支持的编码转换

~~~cpp
std::string UnicodeToUTF8(const wchar_t* str, int strLen);
std::string UnicodeToAnsi(const wchar_t* str, int strLen);
std::wstring AnsiToUnicode(const char* str, int strLen);
std::string AnsiToUtf8(const std::string& str);
std::wstring Utf8ToUnicode(const std::string& str);
~~~

## 文件

[win_str_utils.h](./win_str_utils.h)
[win_str_utils.cpp](./win_str_utils.cpp)

***
_This README was generated with ❤️ by [readme-md-generator](https://github.com/kefranabg/readme-md-generator)_