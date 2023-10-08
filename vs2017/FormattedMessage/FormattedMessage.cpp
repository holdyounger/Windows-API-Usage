// FormattedMessage.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#ifndef UNICODE
#define UNICODE
#endif

#include <windows.h>
#include <stdio.h>

LPWSTR GetFormattedMessage(LPWSTR pMessage, ...);

void main(void)
{
	LPWSTR pBuffer = NULL;
	LPWSTR pMessage = L"%1!*.*s! %3 %4!*s!";

	// The variable length arguments correspond directly to the format
	// strings in pMessage.
	pBuffer = GetFormattedMessage(pMessage, 4, 2, L"Bill", L"Bob", 6, L"Bill");
	if (pBuffer)
	{
		// Buffer contains "  Bi Bob   Bill".
		wprintf(L"Formatted message: %s\n", pBuffer);
		LocalFree(pBuffer);
	}
	else
	{
		wprintf(L"Format message failed with 0x%x\n", GetLastError());
	}
}

// Formats a message string using the specified message and variable
// list of arguments.
LPWSTR GetFormattedMessage(LPWSTR pMessage, ...)
{
	LPWSTR pBuffer = NULL;

	va_list args = NULL;
	va_start(args, pMessage);

	FormatMessage(FORMAT_MESSAGE_FROM_STRING |
		FORMAT_MESSAGE_ALLOCATE_BUFFER,
		pMessage,
		0,
		0,
		(LPWSTR)&pBuffer,
		0,
		&args);

	va_end(args);

	return pBuffer;
}
// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
