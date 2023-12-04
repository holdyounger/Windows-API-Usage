#pragma once
#include <atlstr.h>
#include <windows.h>
#include <psapi.h>

#define DBG_LEVEL_DEBUG 0
#define DBG_LEVEL_INFO 1
#define DBG_LEVEL_TRACE 2
#define DBG_LEVEL_ERROR 3
#define DBG_LEVEL_FILE	4

static const CString g_sDbgStr[] = { L"[Debug]", L"[Info]", L"[Trace]", L"[Error]" };
#define Level_Str(idx)  ( idx < _ARRAYSIZE(g_sDbgStr) ? g_sDbgStr[idx] :  g_sDbgStr[_ARRAYSIZE(g_sDbgStr)-1])


enum RPC_CMD_ERROR {
	RPC_NO_ERROR = 0,
	RPC_WEAK_USER = 1,
	RPC_CMD_UNKNOW,
	RPC_PARSE_ERROR
};

static inline CString GetCurProcessName()
{
	WCHAR process_name[MAX_PATH]{ 0 };
	DWORD len = GetModuleBaseNameW(GetCurrentProcess(), NULL, process_name,
		MAX_PATH);
	if (len > 0) {
		process_name[len] = 0;
	}

	return process_name;
}

static const CString g_StrCurProcessName = GetCurProcessName();

inline BOOL Debug_Print(int level, LPCSTR lpText, ...)
{
	CString strText;
	va_list args;
	va_start(args, lpText);

	if (!g_StrCurProcessName.IsEmpty())
	{
		strText.Append("[");
		strText.Append(g_StrCurProcessName);
		strText.Append("]");
	}

	strText.Append(Level_Str(level));

	strText.AppendFormatV(lpText, args);

	if (level > DBG_LEVEL_DEBUG)
	{
		OutputDebugString(strText);
	}
	else
	{
#ifdef _DEBUG
		OutputDebugString(strText);
#endif
	}

	return true;
}


#define DEBUG_PRINT(levle, ...) Debug_Print(levle, __VA_ARGS__) 

#define Debug_Run(format, ...) DEBUG_PRINT(DBG_LEVEL_DEBUG, format,##__VA_ARGS__)
#define Debug_Info(format, ...) DEBUG_PRINT(DBG_LEVEL_INFO, format, ##__VA_ARGS__)
#define Debug_Trace(format, ...) DEBUG_PRINT(DBG_LEVEL_TRACE, format, ##__VA_ARGS__)
#define Debug_Error(format, ...) DEBUG_PRINT(DBG_LEVEL_ERROR, format, ##__VA_ARGS__)