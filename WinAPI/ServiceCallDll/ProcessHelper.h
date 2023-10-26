#pragma once

#include <windows.h>
#include <tlhelp32.h>
#include <atlstr.h>
#include <iostream>
#include <vector>

namespace Process
{
	BOOL Is64BitProcess(DWORD dwPid);
	BOOL GetEnvBypid(DWORD dwPid, CString& strEnvName, CString& strEnvValue);
	BOOL GetPidsByProcessName(CString process, std::vector<DWORD>& Pids);
	BOOL EnableDebugPrivilege();
}
