#include <windows.h>
#include <atlstr.h>
#include <psapi.h>
#include <iostream>

using namespace std;

static inline CString log_current_process()
{
	WCHAR process_name[MAX_PATH]{ 0 };
	DWORD len = GetModuleBaseNameW(GetCurrentProcess(), NULL, process_name,
		MAX_PATH);
	if (len > 0) {
		process_name[len] = 0;
	}

	return process_name;
}

inline BOOL Debug_Run(LPCWSTR lpText, ...)
{
	CString strText;
	va_list args;
	va_start(args, lpText);

	static CString strProcessName = log_current_process();
	if (!strProcessName.IsEmpty())
	{
		strText.Append(L"[");
		strText.Append(strProcessName);
		strText.Append(L"]");
	}

	strText.AppendFormatV(lpText, args);

	strText.AppendChar(L'\n');

	OutputDebugString(strText);

	va_end(args);
	return TRUE;
}

inline BOOL Debug_Error(LPCWSTR lpText, ...)
{
	CString strText;
	va_list args;
	va_start(args, lpText);

	static CString strProcessName = log_current_process();
	if (!strProcessName.IsEmpty())
	{
		strText.Append(L"[");
		strText.Append(strProcessName);
		strText.Append(L"]");
	}

	strText.Append(L" [Error] ");

	strText.AppendFormatV(lpText, args);

	strText.AppendChar(L'\n');

	OutputDebugString(strText);

	va_end(args);
	return TRUE;
}