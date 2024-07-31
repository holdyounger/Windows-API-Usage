// apr_sha1.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <string>
#include <windows.h>
#include <Shlwapi.h>
#include "apr_sha1.h"

char HexCharArr[] = { '0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f' };
void byteToHex(const apr_byte_t* pData, int nLen, char* pOut)
{
    int index = 0;
    for (int i = 0; i < nLen; i++)
    {
        pOut[index++] = HexCharArr[pData[i] >> 4 & 0x0f];
        pOut[index++] = HexCharArr[pData[i] & 0x0f];
    }
}

BOOL mysql_password_validate(const char* password, const char* hash)
{
    apr_byte_t sFirst[APR_SHA1_DIGESTSIZE] = { 0 };
    apr_sha1_hex(password, strlen(password), sFirst);

    apr_byte_t sSencond[APR_SHA1_DIGESTSIZE] = { 0 };
    apr_sha1_hex((const char*)sFirst, APR_SHA1_DIGESTSIZE, sSencond);

    char sDst[41] = { 0 };
    byteToHex(sSencond, APR_SHA1_DIGESTSIZE, sDst);

    return (StrCmpIA(sDst, hash) == 0) ? TRUE : FALSE;
}

BOOL mysql_password_generate(const char* password, char* hash, int nLen)
{
    apr_byte_t sFirst[APR_SHA1_DIGESTSIZE] = { 0 };
    apr_sha1_hex(password, strlen(password), sFirst);

    apr_byte_t sSencond[APR_SHA1_DIGESTSIZE] = { 0 };
    apr_sha1_hex((const char*)sFirst, APR_SHA1_DIGESTSIZE, sSencond);

    char sDst[41] = { 0 };
    byteToHex(sSencond, APR_SHA1_DIGESTSIZE, sDst);

    strcpy_s(hash, nLen, sDst);

    return TRUE;
}

#define RESET   "\033[0m"
#define BLACK   "\033[30m"      /* Black */
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define YELLOW  "\033[33m"      /* Yellow */
#define BLUE    "\033[34m"      /* Blue */
#define MAGENTA "\033[35m"      /* Magenta */
#define CYAN    "\033[36m"      /* Cyan */
#define WHITE   "\033[37m"      /* White */
#define BOLDBLACK   "\033[1m\033[30m"      /* Bold Black */
#define BOLDRED     "\033[1m\033[31m"      /* Bold Red */
#define BOLDGREEN   "\033[1m\033[32m"      /* Bold Green */
#define BOLDYELLOW  "\033[1m\033[33m"      /* Bold Yellow */
#define BOLDBLUE    "\033[1m\033[34m"      /* Bold Blue */
#define BOLDMAGENTA "\033[1m\033[35m"      /* Bold Magenta */
#define BOLDCYAN    "\033[1m\033[36m"      /* Bold Cyan */
#define BOLDWHITE   "\033[1m\033[37m"      /* Bold White */

#ifndef DETOUR_TRACE
#if DETOUR_DEBUG
#define DETOUR_TRACE(x) printf x
#define DETOUR_BREAK()  __debugbreak()
#include <stdio.h>
#include <limits.h>
#else
// #define DETOUR_TRACE(x)
#include <stdio.h>
#include <string.h>
// 声明一个辅助函数，用于处理可变参数列表
static void DebugTraceA(const char* format, ...) {
    va_list args;
    va_start(args, format);
    char buffer[1024] = { 0 };
    memcpy_s(buffer, 13, "[DNRSP][SDK]", 13);
    vsprintf_s(buffer + 12, sizeof(buffer) - 12, format, args);
    va_end(args);
    OutputDebugStringA(buffer);
}

// 定义宏，使用DebugTraceA函数输出调试信息
#define DETOUR_TRACE(x, ...) DebugTraceA(x, ##__VA_ARGS__)
#define DETOUR_BREAK()
#endif
#endif

int main()
{
    using namespace std;
    DETOUR_TRACE("Debug:%s", "teset");
    std::string pwd;
    char hash[41];
    std::cout << "Sha1 Hash Generate Tool!\n";
    std::cout << "input pwd>";
    std::cin >> pwd;

    while (cin.fail() == false)
    {
        mysql_password_generate(pwd.c_str(), hash, 41);
        std::cout << "Encrypt Result: " << GREEN << hash;
        std::cout << endl;
        std::cout << WHITE << "-----------------------------------------------------";
        cout << endl;
        std::cout << "input pwd>";
        std::cin >> pwd;

    }
}
