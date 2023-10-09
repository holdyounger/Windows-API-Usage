// GetProceccEnv.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
/**************************************************************************
 *
 *        Copyright (C) 2023
 *        All rights reserved
 *
 *        Filename    :	GetProcessEnv.cpp
 *        Description : 获取进程的环境变量
 *
 *        Created by  mingming.shi 2023年10月9日
 *
 *************************************************************************/

#include <iostream>
#include <Windows.h>
#include <atlstr.h>
#include <winternl.h>
#include <psapi.h>
#include <string>
#include <tlhelp32.h>

#pragma comment(lib, "Psapi.lib")

typedef NTSTATUS(NTAPI *_NtQueryInformationProcess)(
	IN HANDLE ProcessHandle,
	ULONG ProcessInformationClass,
	OUT PVOID ProcessInformation,
	IN ULONG ProcessInformationLength,
	OUT PULONG ReturnLength OPTIONAL
	);
typedef NTSTATUS(NTAPI *_NtWow64ReadVirtualMemory64)(
	IN HANDLE ProcessHandle,
	IN PVOID64 BaseAddress,
	OUT PVOID Buffer,
	IN ULONG64 Size,
	OUT PULONG64 NumberOfBytesRead);

typedef struct _PROCESS_BASIC_INFORMATION_WOW64 {
	PVOID Reserved1[2];
	PVOID64 PebBaseAddress;
	PVOID Reserved2[4];
	ULONG_PTR UniqueProcessId[2];
	PVOID Reserved3[2];
} PROCESS_BASIC_INFORMATION_WOW64;

typedef struct
{
	PVOID64 Filler[3];
	PVOID64 Ldr;
	PVOID64 ProcessParameters;
} __PEB64;

typedef struct _UNICODE_STRING_WOW64 {
	USHORT Length;
	USHORT MaximumLength;
	PVOID64 Buffer;
} UNICODE_STRING_WOW64;

typedef struct _CURDIR_WOW64
{
	UNICODE_STRING_WOW64 DosPath;
	PVOID64 Handle;
} CURDIR_WOW64, *PCURDIR_WOW64;

typedef struct _RTL_DRIVE_LETTER_CURDIR
{
	WORD Flags;
	WORD Length;
	ULONG TimeStamp;
	STRING DosPath;
} RTL_DRIVE_LETTER_CURDIR, *PRTL_DRIVE_LETTER_CURDIR;

typedef struct _RTL_USER_PROCESS_PARAMETERS_X64               // 30 elements, 0x400 bytes (sizeof)
{
	/*0x000*/     ULONG32      MaximumLength;
	/*0x004*/     ULONG32      Length;
	/*0x008*/     ULONG32      Flags;
	/*0x00C*/     ULONG32      DebugFlags;
	/*0x010*/     PVOID64      ConsoleHandle;
	/*0x018*/     ULONG32      ConsoleFlags;
	/*0x01C*/     UINT8        _PADDING0_[0x4];
	/*0x020*/     PVOID64        StandardInput;
	/*0x028*/     PVOID64        StandardOutput;
	/*0x030*/     PVOID64        StandardError;
	/*0x038*/     struct _CURDIR_WOW64 CurrentDirectory;                       // 2 elements, 0x18 bytes (sizeof)
	/*0x050*/     struct _UNICODE_STRING_WOW64 DllPath;                        // 3 elements, 0x10 bytes (sizeof)
	/*0x060*/     struct _UNICODE_STRING_WOW64 ImagePathName;                  // 3 elements, 0x10 bytes (sizeof)
	/*0x070*/     struct _UNICODE_STRING_WOW64 CommandLine;                    // 3 elements, 0x10 bytes (sizeof)
	/*0x080*/     PVOID64        Environment;
	/*0x088*/     ULONG32      StartingX;
	/*0x08C*/     ULONG32      StartingY;
	/*0x090*/     ULONG32      CountX;
	/*0x094*/     ULONG32      CountY;
	/*0x098*/     ULONG32      CountCharsX;
	/*0x09C*/     ULONG32      CountCharsY;
	/*0x0A0*/     ULONG32      FillAttribute;
	/*0x0A4*/     ULONG32      WindowFlags;
	/*0x0A8*/     ULONG32      ShowWindowFlags;
	/*0x0AC*/     UINT8        _PADDING1_[0x4];
	/*0x0B0*/     struct _UNICODE_STRING_WOW64 WindowTitle;                    // 3 elements, 0x10 bytes (sizeof)
	/*0x0C0*/     struct _UNICODE_STRING_WOW64 DesktopInfo;                    // 3 elements, 0x10 bytes (sizeof)
	/*0x0D0*/     struct _UNICODE_STRING_WOW64 ShellInfo;                      // 3 elements, 0x10 bytes (sizeof)
	/*0x0E0*/     struct _UNICODE_STRING_WOW64 RuntimeData;                    // 3 elements, 0x10 bytes (sizeof)
	/*0x0F0*/     struct _RTL_DRIVE_LETTER_CURDIR CurrentDirectores[32];
}RTL_USER_PROCESS_PARAMETERS_X64, *PRTL_USER_PROCESS_PARAMETERS_X64;

BOOL GetProcessEnvByPid_Wow64(DWORD dwPid, CString & strEnvName, CString & strEnvValue)
{
	BOOL bRet = FALSE;
	HANDLE hProcess = NULL;
	PROCESS_BASIC_INFORMATION_WOW64 pbi = {};
	ULONG ProcPramFlags = 0;
	static HMODULE hNtdll = NULL;
	static _NtQueryInformationProcess fnNtQueryInformationProcess = NULL;
	static _NtWow64ReadVirtualMemory64 fnNtReadVirtualMemory = NULL;
	do
	{
		if (!dwPid)
			break;

		if (!hNtdll)
		{
			hNtdll = GetModuleHandle(L"ntdll.dll");
		}

		if (!fnNtQueryInformationProcess)
		{
			fnNtQueryInformationProcess = (_NtQueryInformationProcess)GetProcAddress(hNtdll, "NtWow64QueryInformationProcess64");

			if (!fnNtQueryInformationProcess)
			{
				break;
			}
		}

		if (!fnNtReadVirtualMemory)
		{
			fnNtReadVirtualMemory = (_NtWow64ReadVirtualMemory64)GetProcAddress(hNtdll, "NtWow64ReadVirtualMemory64");

			if (!fnNtReadVirtualMemory)
			{
				break;
			}
		}

		//需要QUERYINFO和READVM
		hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, dwPid);
		if (!hProcess)
		{
			printf("[OpenProcess] Cannot open process %u", GetLastError());
			break;
		}


		DWORD dwSize = 0;

		LONG status = fnNtQueryInformationProcess(hProcess, ProcessBasicInformation, &pbi, sizeof(pbi), &dwSize);
		if (status != 0)
		{
			printf("[NtWow64QueryInformationProcess64] NtQueryInformationProcess fail = [%08x]", status);
			break;
		}

		PVOID64 pAddrPEB = pbi.PebBaseAddress;
		__PEB64 PEB = {};
		UINT64 nSize = dwSize;
		if (ERROR_SUCCESS != fnNtReadVirtualMemory(hProcess, pAddrPEB, &PEB, sizeof(PEB), &nSize))
		{
			printf("[NtWow64ReadVirtualMemory64] pfnNtReadVirtualMemory PEB fail = [%08x]", status);
			break;
		}

		_RTL_USER_PROCESS_PARAMETERS_X64 RtlUserProcessParameters = {};
		if (ERROR_SUCCESS != fnNtReadVirtualMemory(hProcess, PEB.ProcessParameters, &RtlUserProcessParameters, sizeof(RtlUserProcessParameters), &nSize))
		{
			printf("[NtWow64ReadVirtualMemory64] pfnNtReadVirtualMemory RtlUserProcessParameters fail = [%08x]", status);
			break;
		}

		WCHAR sTmpEnv[1025] = { 0 };
		ULONG uTotalSize = 0;
		PVOID64 uPtrBegin = RtlUserProcessParameters.Environment;
		while (ERROR_SUCCESS == fnNtReadVirtualMemory(hProcess, uPtrBegin, sTmpEnv, 1024, &nSize))
		{
			uTotalSize += nSize;
			uPtrBegin = (PVOID64)((ULONGLONG)(uPtrBegin)+nSize);
		}

		printf("[%S] env size : %d", __FUNCTIONW__, uTotalSize);

		WCHAR* lpEnvironment = new WCHAR[uTotalSize + 1];
		if (lpEnvironment)
		{
			memset(lpEnvironment, 0, (uTotalSize + 1) * sizeof(WCHAR));

			if (ERROR_SUCCESS == fnNtReadVirtualMemory(hProcess, RtlUserProcessParameters.Environment, lpEnvironment, uTotalSize, &nSize))
			{
				WCHAR *ptr = lpEnvironment;
				while (*ptr != L'\0') {
					printf("[%S] env str : %S\n", __FUNCTIONW__, ptr);
					if (StrCmpNIW(strEnvName, ptr, strEnvName.GetLength()) == 0)
					{
						strEnvValue = ptr + strEnvName.GetLength() + 1;
						strEnvValue.Trim();
						printf("{%S] target : %S, value : %d", __FUNCTIONW__, strEnvName, strEnvValue);
						break;
					}

					ptr += wcslen(ptr) + 1;
				}
				bRet = TRUE;
			}
			else
			{
				printf("[GetProcessInfoByPid_Wow64] Readprocmemory Environment %08x failed %u ", RtlUserProcessParameters.CommandLine.Buffer, GetLastError());
			}

			delete[] lpEnvironment;
		}

	} while (FALSE);

	if (hProcess)
	{
		CloseHandle(hProcess);
	}

	return bRet;
}

typedef BOOL(WINAPI *LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);
BOOL Is64BitProcess(DWORD dwPid)
{
	BOOL bRet = FALSE;
	if (dwPid)
	{
		HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, dwPid);
		if (hProcess)
		{

			BOOL bIsWow64 = FALSE;
			static LPFN_ISWOW64PROCESS fnIsWow64Process = NULL;

			if (!fnIsWow64Process)
			{
				fnIsWow64Process = (LPFN_ISWOW64PROCESS)GetProcAddress(GetModuleHandle(TEXT("kernel32")), "IsWow64Process");
			}

			if (NULL != fnIsWow64Process)
			{
				if (fnIsWow64Process(hProcess, &bIsWow64))
				{
					if (bIsWow64 == FALSE)
						bRet = TRUE;
				}
			}
			CloseHandle(hProcess);
		}
	}
	return bRet;
}

typedef struct
{
	PVOID Filler[4];
	PVOID ProcessParameters;
} __PEB;
typedef struct _CURDIR
{
	UNICODE_STRING DosPath;
	PVOID Handle;
} CURDIR, *PCURDIR;

typedef struct _RTL_USER_PROCESS_PARAMETERS_X86                // 30 elements, 0x298 bytes (sizeof)
{
	/*0x000*/     ULONG32      MaximumLength;
	/*0x004*/     ULONG32      Length;
	/*0x008*/     ULONG32      Flags;
	/*0x00C*/     ULONG32      DebugFlags;
	/*0x010*/     VOID*        ConsoleHandle;
	/*0x014*/     ULONG32      ConsoleFlags;
	/*0x018*/     VOID*        StandardInput;
	/*0x01C*/     VOID*        StandardOutput;
	/*0x020*/     VOID*        StandardError;
	/*0x024*/     struct _CURDIR CurrentDirectory;                       // 2 elements, 0xC bytes (sizeof)
	/*0x030*/     struct _UNICODE_STRING DllPath;                        // 3 elements, 0x8 bytes (sizeof)
	/*0x038*/     struct _UNICODE_STRING ImagePathName;                  // 3 elements, 0x8 bytes (sizeof)
	/*0x040*/     struct _UNICODE_STRING CommandLine;                    // 3 elements, 0x8 bytes (sizeof)
	/*0x048*/     VOID*        Environment;
	/*0x04C*/     ULONG32      StartingX;
	/*0x050*/     ULONG32      StartingY;
	/*0x054*/     ULONG32      CountX;
	/*0x058*/     ULONG32      CountY;
	/*0x05C*/     ULONG32      CountCharsX;
	/*0x060*/     ULONG32      CountCharsY;
	/*0x064*/     ULONG32      FillAttribute;
	/*0x068*/     ULONG32      WindowFlags;
	/*0x06C*/     ULONG32      ShowWindowFlags;
	/*0x070*/     struct _UNICODE_STRING WindowTitle;                    // 3 elements, 0x8 bytes (sizeof)
	/*0x078*/     struct _UNICODE_STRING DesktopInfo;                    // 3 elements, 0x8 bytes (sizeof)
	/*0x080*/     struct _UNICODE_STRING ShellInfo;                      // 3 elements, 0x8 bytes (sizeof)
	/*0x088*/     struct _UNICODE_STRING RuntimeData;                    // 3 elements, 0x8 bytes (sizeof)
	/*0x090*/     struct _RTL_DRIVE_LETTER_CURDIR CurrentDirectores[32];
}RTL_USER_PROCESS_PARAMETERS_X86, *PRTL_USER_PROCESS_PARAMETERS_X86;

BOOL GetProcessEnvByPid_x86(DWORD dwPid, CString & strEnvName, CString & strEnvValue)
{
	BOOL bRet = FALSE;
	HANDLE hProcess = NULL;
	PROCESS_BASIC_INFORMATION pbi = {};
	ULONG ProcPramFlags = 0;
	static HMODULE hNtdll = NULL;
	static _NtQueryInformationProcess fnNtQueryInformationProcess = NULL;
	do
	{
		if (!dwPid)
			break;

		if (!hNtdll)
		{
			hNtdll = GetModuleHandle(L"ntdll.dll");
		}

		if (!fnNtQueryInformationProcess)
		{
			fnNtQueryInformationProcess = (_NtQueryInformationProcess)GetProcAddress(hNtdll, "NtQueryInformationProcess");

			if (!fnNtQueryInformationProcess)
			{
				break;
			}
		}

		//需要QUERYINFO和READVM
		hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, dwPid);
		if (!hProcess)
		{
			printf("[C360ProcessHelper::GetProcessInfoByPid_x86] Cannot open process %u", GetLastError());
			break;
		}


		DWORD dwSize = 0;

		LONG status = fnNtQueryInformationProcess(hProcess, ProcessBasicInformation, &pbi, sizeof(pbi), &dwSize);
		if (status != 0)
		{
			printf("[C360ProcessHelper::GetProcessInfoByPid_x86] NtQueryInformationProcess fail = [%08x]", status);
			break;
		}

		PVOID pAddrPEB = pbi.PebBaseAddress;
		__PEB PEB = {};
		SIZE_T nSize = dwSize;
		if (!ReadProcessMemory(hProcess, pAddrPEB, &PEB, sizeof(PEB), &nSize))
		{
			printf("[C360ProcessHelper::GetProcessInfoByPid_x86] pfnNtReadVirtualMemory PEB fail = [%08x]", status);
			break;
		}

		_RTL_USER_PROCESS_PARAMETERS_X86 RtlUserProcessParameters = {};
		if (!ReadProcessMemory(hProcess, (LPVOID)PEB.ProcessParameters, &RtlUserProcessParameters, sizeof(RtlUserProcessParameters), &nSize))
		{
			printf("[C360ProcessHelper::GetProcessInfoByPid_x86] pfnNtReadVirtualMemory RtlUserProcessParameters fail = [%08x]", status);
			break;
		}

		WCHAR sTmpEnv[1025] = { 0 };
		ULONG uTotalSize = 0;
		PVOID uPtrBegin = RtlUserProcessParameters.Environment;
		while (ReadProcessMemory(hProcess, (LPVOID)RtlUserProcessParameters.Environment, uPtrBegin, 1024, &nSize))
		{
			uTotalSize += nSize;
			uPtrBegin = (PVOID)((ULONG)(uPtrBegin)+nSize);
		}

		printf("[%s] env size : %d", __FUNCTIONW__, uTotalSize);

		WCHAR* lpEnvironment = new WCHAR[uTotalSize + 1];
		if (lpEnvironment)
		{
			memset(lpEnvironment, 0, (uTotalSize + 1) * sizeof(WCHAR));

			if (ReadProcessMemory(hProcess, (LPVOID)RtlUserProcessParameters.Environment, lpEnvironment, uTotalSize, &nSize))
			{
				WCHAR *ptr = lpEnvironment;
				while (*ptr != L'\0') {
					ATLTRACE(L"[%s] env str : %s", __FUNCTIONW__, ptr);
					if (StrCmpNIW(strEnvName, ptr, strEnvName.GetLength()) == 0)
					{
						strEnvValue = ptr + strEnvName.GetLength() + 1;
						strEnvValue.Trim();
						printf("{%s] target : %s, value : %d", __FUNCTIONW__, strEnvName, strEnvValue);
						break;
					}

					ptr += wcslen(ptr) + 1;
				}
				bRet = TRUE;
			}
			else
			{
				printf("[C360ProcessHelper::GetProcessInfoByPid_x86] Readprocmemory Environment %08x failed %u ", RtlUserProcessParameters.Environment, GetLastError());
			}
			delete[] lpEnvironment;
		}
	} while (FALSE);

	if (hProcess)
	{
		CloseHandle(hProcess);
	}

	return bRet;
}


BOOL GetEnvByPid(DWORD dwPid, CString & strEnvName, CString & strEnvValue)
{
	BOOL bRet = FALSE;
	if (Is64BitProcess(dwPid))
	{
		if (GetProcessEnvByPid_Wow64(dwPid, strEnvName, strEnvValue))
		{
			bRet = TRUE;
		}
	}
	else
	{
		if (GetProcessEnvByPid_x86(dwPid, strEnvName, strEnvValue))
		{
			bRet = TRUE;
		}
	}
	return bRet;
}


BOOL GetProcessHomeDir(CString &szQueryName, CString &strResult)
{
	BOOL bRet = FALSE;
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hSnapshot == INVALID_HANDLE_VALUE)
	{
		printf("[%s] create snap failed(%d)!!!", __FUNCTIONW__, GetLastError());
		return bRet;
	}

	do
	{
		PROCESSENTRY32 pe32;
		pe32.dwSize = sizeof(PROCESSENTRY32);
		if (!Process32First(hSnapshot, &pe32))
		{
			printf("[%s] process first failed!!!", __FUNCTIONW__);
			break;
		}

		do
		{
			if (StrStrIW(pe32.szExeFile, szQueryName) != NULL)
			{
				std::cout << "Input Query Env Variable:";
				CString strEnvName = "";
				std::wstring wsInput;
				std::cout << "EnvName:";
				std::wcin >> wsInput;

				strEnvName.Format(L"%s", wsInput.c_str());
				CString sEnvName = L"JENKINS_HOME";
				GetEnvByPid(pe32.th32ProcessID, strEnvName, strResult);
				break;
			}
		} while (Process32Next(hSnapshot, &pe32));

	} while (FALSE);

	CloseHandle(hSnapshot);

	return bRet;
}

int main()
{
    std::cout << "Hello World!\n";

	do{

		CString strProcessName = "";
		CString strResult = "";

		std::wstring wsInput;
		std::cout << "ProcessName:";
		std::wcin >> wsInput;

		strProcessName.Format(L"%s", wsInput.c_str());
		GetProcessHomeDir(strProcessName, strResult);

		printf("Value: %S\n", strResult.GetString());
		printf("----------------------\n");

	} while (std::cin);

#if 0
	do {

		DWORD dwpid;
		CString strEnvName = "";
		CString strEnvValue = "";

		std::wstring wsInput;
		std::cout << "Pid:";
		std::cin >> dwpid;
		std::cout << "EnvName:";
		std::wcin >> wsInput;

		strEnvName.Format(L"%s", wsInput.c_str());
		printf("EnvName: %S \n", strEnvName.GetString());

		GetEnvByPid(dwpid, strEnvName, strEnvValue);

		printf("EnvName: %S \nValue: %S\n", strEnvName.GetString(), strEnvValue.GetString());
		printf("----------------------\n");

	} while (std::cin);
#endif

	return 0;
}