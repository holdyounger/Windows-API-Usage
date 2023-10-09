// dumpcreate.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <windows.h>
#include <dbghelp.h>  
#include <stdio.h>
#include <string.h>
#include <tchar.h>
#include <strsafe.h>
#include <VersionHelpers.h>
#include "./dump_create/generate_mini_dump_win.h"

CInstallCrashDump g_installDump;


LONG WINAPI GWUnhandledExceptionFilter(struct _EXCEPTION_POINTERS* ExceptionInfo)
{

	LONG ret = EXCEPTION_CONTINUE_SEARCH;

	TCHAR szExePath[MAX_PATH] = { 0 };
	// TCHAR szInput[MAX_PATH] = _T("%programdata%\\TrustAgent\\dumps\\");

	// ExpandEnvironmentStrings(szInput, szExePath, MAX_PATH);

	if (::GetModuleFileName(NULL, szExePath, MAX_PATH) > 0)
	{
		int ch = _T('\\');
		*_tcsrchr(szExePath, ch) = _T('\0');
#ifdef _UNICODE
		wcscat_s(szExePath, sizeof(szExePath) / sizeof(TCHAR), _T("\\AgentService.dmp"));
#else
		tcsncat_s(dest, sizeof(dest) / sizeof(TCHAR), src, _TRUNCATE)
#endif
	}

	HANDLE hFile = ::CreateFile(szExePath, GENERIC_WRITE,
		FILE_SHARE_WRITE, NULL, CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile != INVALID_HANDLE_VALUE)
	{
		MINIDUMP_EXCEPTION_INFORMATION exptInfo;
		exptInfo.ThreadId = ::GetCurrentThreadId();
		exptInfo.ExceptionPointers = ExceptionInfo;

		BOOL bOK = ::MiniDumpWriteDump(::GetCurrentProcess(),
			::GetCurrentProcessId(),
			hFile, MiniDumpNormal,
			&exptInfo, NULL, NULL);
		if (bOK)
			ret = EXCEPTION_EXECUTE_HANDLER;
		CloseHandle(hFile);
	}

	printf("Unhandled Exception(0x%X @ %p),MiniDump:\"%s\"",
		ExceptionInfo->ExceptionRecord->ExceptionCode,
		ExceptionInfo->ExceptionRecord->ExceptionAddress, szExePath);
	return EXCEPTION_EXECUTE_HANDLER;

}

void DisableSetUnhandledExceptionFilter()
{
	void* addr = (void*)GetProcAddress(LoadLibrary(_T("kernel32.dll")),
		"SetUnhandledExceptionFilter");

	if (addr)
	{
		unsigned char code[16];
		int size = 0;

		code[size++] = 0x33;
		code[size++] = 0xC0;
		code[size++] = 0xC2;
		code[size++] = 0x04;
		code[size++] = 0x00;

		DWORD dwOldFlag, dwTempFlag;
		VirtualProtect(addr, size, PAGE_READWRITE, &dwOldFlag);
		WriteProcessMemory(GetCurrentProcess(), addr, code, size, NULL);
		VirtualProtect(addr, size, dwOldFlag, &dwTempFlag);
	}
}

bool IsWindowsNT()
{
	OSVERSIONINFOEX osvi;
	ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
	// osvi.dwPlatformId = VER_PLATFORM_WIN32s;
	// osvi.dwPlatformId = VER_PLATFORM_WIN32_WINDOWS;
	osvi.dwPlatformId = VER_PLATFORM_WIN32_NT;

	DWORDLONG conditionMask = 0;
	VER_SET_CONDITION(conditionMask, VER_PLATFORMID, VER_EQUAL);

	return  VerifyVersionInfo(&osvi, VER_PLATFORMID, conditionMask);
}

int main()
{
	// 设置程序运行的兼容性模式为 Windows XP
	SetDllDirectory(L"");  // 在 Windows XP 下运行必须调用此函数

    std::cout << "Hello World!\n";

	DWORD d1 = HIBYTE(_WIN32_WINNT_WS03);
	DWORD d2 = LOBYTE(_WIN32_WINNT_WS03);
	DWORD d3 = OSVER(NTDDI_WS03SP1);
	DWORD d4 = SUBVER(NTDDI_WS03SP1);
	DWORD d5 = SPVER(NTDDI_WS03SP1);

	bool platformId = IsWindowsNT();

	// if (IsWindowsVersionOrGreater(OSVER(NTDDI_WS03SP1), SUBVER(NTDDI_WS03SP1), SPVER(NTDDI_WS03SP1)))

	if (IsWindowsVersionOrGreater(OSVER(NTDDI_WIN7), SUBVER(NTDDI_WIN7), SPVER(NTDDI_WIN7)))
	{
		std::cout << "Hello World!\n";
	}
	else
	{
		std::cout << "Hello World!\n";
	}

	DisableSetUnhandledExceptionFilter();
	SetUnhandledExceptionFilter(GWUnhandledExceptionFilter);

	// 触发异常
	int* p = nullptr;
	*p = 100; // 这里会触发空指针异常
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
