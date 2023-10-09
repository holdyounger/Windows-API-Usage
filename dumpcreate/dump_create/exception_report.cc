#include "stdafx.h"
#include "exception_report.h"
#include <Strsafe.h>
#include <VersionHelpers.h>
#pragma comment(lib, "Dbghelp.lib")

#ifdef SP_OEM_ZEROTRUST
bool IsWindowsNT()
{
	OSVERSIONINFOEX osvi;
	ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
	osvi.dwPlatformId = VER_PLATFORM_WIN32_NT;

	DWORDLONG conditionMask = 0;
	VER_SET_CONDITION(conditionMask, VER_PLATFORMID, VER_EQUAL);

	return VerifyVersionInfo(&osvi, VER_PLATFORMID, conditionMask);
}
#endif

CExceptionReport::CExceptionReport(PEXCEPTION_POINTERS pExceptionInfo) : m_pException(pExceptionInfo) {
	TCHAR szModuleName[MAX_PATH];
	DWORD dwLen = ::GetModuleFileName(NULL, szModuleName, MAX_PATH);
	m_strAppPath = dwLen != 0 ? szModuleName : _T("");

#ifdef SP_OEM_ZEROTRUST
	TCHAR szAppDataDir[MAX_PATH];
	dwLen = ExpandEnvironmentStrings(_T("%programdata%"), szAppDataDir, MAX_PATH);
	if (dwLen != 0) {
		StringCchCat(szAppDataDir, sizeof(szAppDataDir), _T("\\TrustAgent\\dumps\\"));
	}
	else {
		dwLen = GetWindowsDirectory(szAppDataDir, MAX_PATH);
		if (dwLen != 0) {
			StringCchCat(szAppDataDir, sizeof(szAppDataDir), _T("\\Temp\\qianxin\\TrustAgent\\"));
		}
	}
#else
	TCHAR szAppDataDir[MAX_PATH];
	dwLen = ::GetEnvironmentVariable(_T("TEMP"), szAppDataDir, MAX_PATH);
	if (dwLen != 0) {
		StringCchCat(szAppDataDir, sizeof(szAppDataDir), _T("\\qianxin"));
	} else {
		GetWindowsDirectory(szAppDataDir, MAX_PATH);
		StringCchCat(szAppDataDir, sizeof(szAppDataDir), _T("\\Temp\\qianxin"));
	}
#endif // SP_OEM_ZEROTRUST

	::CreateDirectory(szAppDataDir, NULL);
	m_strReportDir = szAppDataDir;

	m_bMaxDmp = false;
	m_bVistaLater = false;

	m_bXpSp2OrLater = FALSE;
	m_b2003Sp1OrLater = FALSE;


#ifdef SP_OEM_ZEROTRUST
	// 系统版本号参见： https://learn.microsoft.com/zh-cn/windows-hardware/drivers/ddi/wdm/ns-wdm-_osversioninfoexw
	bool IsWinNTplatformId = IsWindowsNT();

	if (IsWinNTplatformId && IsWindowsVistaOrGreater())
		m_bVistaLater = true;

	if (m_bVistaLater)
	{
		m_bXpSp2OrLater = TRUE;
		m_b2003Sp1OrLater = TRUE;
	}
	else
	{
		if (IsWinNTplatformId)
		{
			if (IsWindowsVersionOrGreater(OSVER(NTDDI_WINXPSP2), SUBVER(NTDDI_WINXPSP2), SPVER(NTDDI_WINXPSP2)))
			{
				m_bXpSp2OrLater = TRUE;
			}

			if (IsWindowsVersionOrGreater(OSVER(NTDDI_WS03SP1), SUBVER(NTDDI_WS03SP1), SPVER(NTDDI_WS03SP1)))
			{
				m_b2003Sp1OrLater = TRUE;
			}
		}
	}
#else
	OSVERSIONINFO osver;	
	osver.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

	if(GetVersionEx(&osver) && osver.dwPlatformId == VER_PLATFORM_WIN32_NT && (osver.dwMajorVersion >= 6))
		m_bVistaLater = true;

	if(m_bVistaLater)
	{
		m_bXpSp2OrLater = TRUE;
		m_b2003Sp1OrLater = TRUE;
	}
	else
	{
		OSVERSIONINFOEX osver2 = {};
		osver2.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
		if(GetVersionEx((LPOSVERSIONINFO)&osver2) && osver2.dwPlatformId == VER_PLATFORM_WIN32_NT)
		{
			if(osver2.dwMajorVersion == 5)
			{
				if(osver2.dwMinorVersion == 1 && osver2.wServicePackMajor >= 2)
					m_bXpSp2OrLater = TRUE;

				if(osver2.dwMinorVersion == 2 && osver2.wServicePackMajor >= 1)
					m_b2003Sp1OrLater = TRUE;
			}
		}
	}
#endif // SP_OEM_ZERTTRUST
}

void CExceptionReport::WriteDumpFile(LPCTSTR lpszFileName) {

	//获取系统版本号，获取dbghelp.dll的版本号
	//(os>=2003 sp1 || os>=xp Sp3) && dgbhelp>5.1 才支持 MiniDumpWithUnloadedModules

	BOOL bSupport_MiniDumpWithUnloadedModules = FALSE;

	if(m_b2003Sp1OrLater || m_bXpSp2OrLater)
		bSupport_MiniDumpWithUnloadedModules = TRUE;

	MINIDUMP_TYPE e_dumpType = (MINIDUMP_TYPE)0;

	if(m_bVistaLater)
		e_dumpType = (MINIDUMP_TYPE)(e_dumpType | MiniDumpNormal | MiniDumpWithFullMemoryInfo);
	else
		e_dumpType = (MINIDUMP_TYPE)(e_dumpType | MiniDumpWithFullMemoryInfo | MiniDumpWithIndirectlyReferencedMemory);

	if(bSupport_MiniDumpWithUnloadedModules)
		e_dumpType = (MINIDUMP_TYPE)(e_dumpType | MiniDumpWithUnloadedModules);

	HANDLE hFile = ::CreateFile(lpszFileName, GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile != INVALID_HANDLE_VALUE) {
		MINIDUMP_EXCEPTION_INFORMATION eInfo;
		eInfo.ThreadId = ::GetCurrentThreadId();
		eInfo.ExceptionPointers = m_pException;
		eInfo.ClientPointers = FALSE;

		MINIDUMP_CALLBACK_INFORMATION cbMiniDump;
		cbMiniDump.CallbackRoutine = CExceptionReport::MiniDumpCallback;
		cbMiniDump.CallbackParam = 0;

		BOOL bRes = ::MiniDumpWriteDump(::GetCurrentProcess(), 
			::GetCurrentProcessId(), 
			hFile, 
			e_dumpType,
			m_pException ? &eInfo : NULL, 
			NULL, 
			&cbMiniDump);
		::CloseHandle(hFile);
	}
	//增加FullDump 特殊情况下需要
	e_dumpType = (MINIDUMP_TYPE)(MiniDumpWithFullMemory);

	TCHAR m_szFullDumpPath[MAX_PATH]={};
	_stprintf_s(m_szFullDumpPath, MAX_PATH, _T("%s_full.dmp"), lpszFileName);

	hFile = ::CreateFile(m_szFullDumpPath, GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile != INVALID_HANDLE_VALUE)
	{
		MINIDUMP_EXCEPTION_INFORMATION eInfo;
		eInfo.ThreadId = ::GetCurrentThreadId();
		eInfo.ExceptionPointers = m_pException;
		eInfo.ClientPointers = FALSE;

		MINIDUMP_CALLBACK_INFORMATION cbMiniDump;
		cbMiniDump.CallbackRoutine = CExceptionReport::MiniDumpCallback;
		cbMiniDump.CallbackParam = 0;

		BOOL bRes = ::MiniDumpWriteDump(::GetCurrentProcess(), 
			::GetCurrentProcessId(), 
			hFile, 
			e_dumpType,
			m_pException ? &eInfo : NULL, 
			NULL, 
			&cbMiniDump);
		::CloseHandle(hFile);
	}
}

CString CExceptionReport::GenerateDumpFileName() {
	CString sAppName(m_strAppPath);
	sAppName = sAppName.Mid(sAppName.ReverseFind(_T('\\')) + 1);

	TCHAR szFileName[MAX_PATH];
	for (int i = 0; i<10; i++) 
	{
		StringCbPrintf(szFileName, MAX_PATH, _T("%s\\%s_%03d.dmp"), m_strReportDir, sAppName, i);
		if (!::PathFileExists(szFileName))
			return szFileName;
	}
	m_bMaxDmp = true;
	StringCbPrintf(szFileName, MAX_PATH, _T("%s\\%s_010.dmp"), m_strReportDir, sAppName);
	return szFileName;
}

void CExceptionReport::CatchDump(void)
{
	WriteDumpFile(GenerateDumpFileName());
}

BOOL CALLBACK CExceptionReport::MiniDumpCallback(PVOID CallbackParam, CONST PMINIDUMP_CALLBACK_INPUT CallbackInput, PMINIDUMP_CALLBACK_OUTPUT CallbackOutput)
{
	if (ModuleCallback == CallbackInput->CallbackType) {
	}
	return TRUE;
}

