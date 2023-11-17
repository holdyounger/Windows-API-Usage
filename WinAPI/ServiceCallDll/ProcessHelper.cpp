#include "ProcessHelper.h"
#include "define.h"
#include <winternl.h>
#pragma comment(lib, "Psapi.lib")

using namespace std;

typedef NTSTATUS(NTAPI* LPFN_NTWOW64WRITEVIRTUALMEMORY64)(
	IN HANDLE ProcessHandle,
	IN ULONG64 BaseAddress,
	OUT PVOID BufferData,
	IN ULONG64 BufferLength,
	OUT PULONG64 ReturnLength OPTIONAL);

typedef NTSTATUS(NTAPI* _NtQueryInformationProcess)(
	IN HANDLE ProcessHandle,
	ULONG ProcessInformationClass,
	OUT PVOID ProcessInformation,
	IN ULONG ProcessInformationLength,
	OUT PULONG ReturnLength OPTIONAL
	);
typedef NTSTATUS(NTAPI* _NtWow64ReadVirtualMemory64)(
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


#if 0
typedef enum _PROCESSINFOCLASS {
	ProcessBasicInformation = 0,
	ProcessWow64Information = 26
} PROCESSINFOCLASS;
#endif // 0


typedef struct _UNICODE_STRING_WOW64 {
	USHORT Length;
	USHORT MaximumLength;
	PVOID64 Buffer;
} UNICODE_STRING_WOW64;

typedef struct _CURDIR_WOW64
{
	UNICODE_STRING_WOW64 DosPath;
	PVOID64 Handle;
} CURDIR_WOW64, * PCURDIR_WOW64;

typedef struct _RTL_DRIVE_LETTER_CURDIR
{
	WORD Flags;
	WORD Length;
	ULONG TimeStamp;
	STRING DosPath;
} RTL_DRIVE_LETTER_CURDIR, * PRTL_DRIVE_LETTER_CURDIR;

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
}RTL_USER_PROCESS_PARAMETERS_X64, * PRTL_USER_PROCESS_PARAMETERS_X64;


typedef struct
{
	PVOID64 Filler[3];
	PVOID64 Ldr;
	PVOID64 ProcessParameters;
} __PEB64;

typedef BOOL(WINAPI* LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);
BOOL Process::Is64BitProcess(DWORD dwPid)
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

BOOL GetProcessEnvByPid_Wow64(DWORD dwPid, CString& strEnvName, CString& strEnvValue)
{
	BOOL bRet = FALSE;

	HANDLE hProcess = NULL;
	PROCESS_BASIC_INFORMATION_WOW64 pbi = {};
	ULONG ProcPramFlags = 0;
	static HMODULE hNtdll = NULL;
	static _NtQueryInformationProcess fnNtQueryInformationProcess = NULL;
	static _NtWow64ReadVirtualMemory64 fnNtReadVirtualMemory64 = NULL;

	do {
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
				Debug_Error(L"[NtWow64QueryInformationProcess64] Failed, GetLastError[%d]", GetLastError());
				break;
			}
		}

		if (!fnNtReadVirtualMemory64)
		{
			fnNtReadVirtualMemory64 = (_NtWow64ReadVirtualMemory64)GetProcAddress(hNtdll, "NtWow64ReadVirtualMemory64");
			if (!fnNtReadVirtualMemory64)
			{
				Debug_Error(L"[NtWow64ReadVirtualMemory64] Failed, GetLastError[%d]", GetLastError());
				break;
			}
		}


		// 打开进程
		hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, dwPid);
		if (!hProcess)
		{
			Debug_Error(L"[OpenProcess] Cannot open process[%d], GetLastError[%d]", dwPid, GetLastError());
			break;
		}

		ULONG dwSize = 0;
		LONG status = fnNtQueryInformationProcess(hProcess, ProcessBasicInformation, &pbi, sizeof(pbi), &dwSize);
		if(ERROR_SUCCESS != status)
		{
			Debug_Error(L"[NtWow64QueryInformationProcess64] NtQueryInformationProcess fail = [%08x]", status);
		}

		PVOID64 pAddrPEB = pbi.PebBaseAddress;
		__PEB64 PEB = {};
		UINT64 nSize = dwSize;
		if (ERROR_SUCCESS != fnNtReadVirtualMemory64(hProcess, pAddrPEB, &PEB, sizeof(PEB), &nSize))
		{
			Debug_Error(L"[NtWow64ReadVirtualMemory64] pfnNtReadVirtualMemory PEB fail = [%08x]", status);
			break;
		}

		_RTL_USER_PROCESS_PARAMETERS_X64 RtlUserProcessParameters = {};
		if (ERROR_SUCCESS != fnNtReadVirtualMemory64(hProcess, PEB.ProcessParameters, &RtlUserProcessParameters, sizeof(RtlUserProcessParameters), &nSize))
		{
			Debug_Error(L"[NtWow64ReadVirtualMemory64] pfnNtReadVirtualMemory RtlUserProcessParameters fail = [%08x]", status);
			break;
		}

		WCHAR sTmpEnv[1025] = { 0 };
		ULONG uTotalSize = 0;
		PVOID64 uPtrBegin = RtlUserProcessParameters.Environment;
		while (ERROR_SUCCESS == fnNtReadVirtualMemory64(hProcess, uPtrBegin, sTmpEnv, 1024, &nSize))
		{
			uTotalSize += nSize;
			uPtrBegin = (PVOID64)((ULONGLONG)(uPtrBegin)+nSize);
		}
		Debug_Error(L"[%S] env size : %d", __FUNCTIONW__, uTotalSize);

		WCHAR* lpEnvironment = new WCHAR[uTotalSize + 1];
		if (lpEnvironment)
		{
			memset(lpEnvironment, 0, (uTotalSize + 1) * sizeof(WCHAR));

			if (ERROR_SUCCESS == fnNtReadVirtualMemory64(hProcess, RtlUserProcessParameters.Environment, lpEnvironment, uTotalSize, &nSize))
			{
				WCHAR* ptr = lpEnvironment;
				while (*ptr != L'\0') {
					Debug_Run(L"[%S] env str : %S\n", __FUNCTIONW__, ptr);
					if (StrCmpNIW(strEnvName, ptr, strEnvName.GetLength()) == 0)
					{
						strEnvValue = ptr + strEnvName.GetLength() + 1;
						strEnvValue.Trim();
						Debug_Error(L"{%S] target : %S, value : %d", __FUNCTIONW__, strEnvName, strEnvValue);
						bRet = TRUE;
						break;
					}

					ptr += wcslen(ptr) + 1;
				}
			}
			else
			{
				Debug_Error(L"[GetProcessInfoByPid_Wow64] Readprocmemory Environment %08x failed %u ", RtlUserProcessParameters.CommandLine.Buffer, GetLastError());
			}

			delete[] lpEnvironment;
		}
	} while (FALSE);

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
} CURDIR, * PCURDIR;

typedef struct _RTL_USER_PROCESS_PARAMETERS_X86                // 30 elements, 0x298 bytes (sizeof)
{
	/*0x000*/     ULONG32      MaximumLength;
	/*0x004*/     ULONG32      Length;
	/*0x008*/     ULONG32      Flags;
	/*0x00C*/     ULONG32      DebugFlags;
	/*0x010*/     VOID* ConsoleHandle;
	/*0x014*/     ULONG32      ConsoleFlags;
	/*0x018*/     VOID* StandardInput;
	/*0x01C*/     VOID* StandardOutput;
	/*0x020*/     VOID* StandardError;
	/*0x024*/     struct _CURDIR CurrentDirectory;                       // 2 elements, 0xC bytes (sizeof)
	/*0x030*/     struct _UNICODE_STRING DllPath;                        // 3 elements, 0x8 bytes (sizeof)
	/*0x038*/     struct _UNICODE_STRING ImagePathName;                  // 3 elements, 0x8 bytes (sizeof)
	/*0x040*/     struct _UNICODE_STRING CommandLine;                    // 3 elements, 0x8 bytes (sizeof)
	/*0x048*/     VOID* Environment;
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
}RTL_USER_PROCESS_PARAMETERS_X86, * PRTL_USER_PROCESS_PARAMETERS_X86;


BOOL GetProcessEnvByPid_x86(DWORD dwPid, CString& strEnvName, CString& strEnvValue)
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
			Debug_Error(L"[C360ProcessHelper::GetProcessInfoByPid_x86] Cannot open process %u", GetLastError());
			break;
		}


		DWORD dwSize = 0;

		LONG status = fnNtQueryInformationProcess(hProcess, ProcessBasicInformation, &pbi, sizeof(pbi), &dwSize);
		if (status != 0)
		{
			Debug_Error(L"[C360ProcessHelper::GetProcessInfoByPid_x86] NtQueryInformationProcess fail = [%08x]", status);
			break;
		}

		PVOID pAddrPEB = pbi.PebBaseAddress;
		__PEB PEB = {};
		SIZE_T nSize = dwSize;
		if (!ReadProcessMemory(hProcess, pAddrPEB, &PEB, sizeof(PEB), &nSize))
		{
			Debug_Error(L"[C360ProcessHelper::GetProcessInfoByPid_x86] pfnNtReadVirtualMemory PEB fail = [%08x]", status);
			break;
		}

		_RTL_USER_PROCESS_PARAMETERS_X86 RtlUserProcessParameters = {};
		if (!ReadProcessMemory(hProcess, (LPVOID)PEB.ProcessParameters, &RtlUserProcessParameters, sizeof(RtlUserProcessParameters), &nSize))
		{
			Debug_Error(L"[C360ProcessHelper::GetProcessInfoByPid_x86] pfnNtReadVirtualMemory RtlUserProcessParameters fail = [%08x]", status);
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

		Debug_Run(L"[%s] env size : %d", __FUNCTIONW__, uTotalSize);

		WCHAR* lpEnvironment = new WCHAR[uTotalSize + 1];
		if (lpEnvironment)
		{
			memset(lpEnvironment, 0, (uTotalSize + 1) * sizeof(WCHAR));

			if (ReadProcessMemory(hProcess, (LPVOID)RtlUserProcessParameters.Environment, lpEnvironment, uTotalSize, &nSize))
			{
				WCHAR* ptr = lpEnvironment;
				while (*ptr != L'\0') {
					ATLTRACE(L"[%s] env str : %s", __FUNCTIONW__, ptr);
					if (StrCmpNIW(strEnvName, ptr, strEnvName.GetLength()) == 0)
					{
						strEnvValue = ptr + strEnvName.GetLength() + 1;
						strEnvValue.Trim();
						Debug_Run(L"{%s] target : %s, value : %d", __FUNCTIONW__, strEnvName, strEnvValue);
						break;
					}

					ptr += wcslen(ptr) + 1;
				}
				bRet = TRUE;
			}
			else
			{
				Debug_Error(L"[C360ProcessHelper::GetProcessInfoByPid_x86] Readprocmemory Environment %08x failed %u ", RtlUserProcessParameters.Environment, GetLastError());
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

BOOL Process::GetEnvBypid(DWORD dwPid, CString& strEnvName, CString& strEnvValue)
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
		if (GetProcessEnvByPid_x86(dwPid, strEnvValue, strEnvValue))
		{
			bRet = TRUE;
		}
	}

	return bRet;
}

BOOL Process::GetPidsByProcessName(CString process, vector<DWORD>& Pids)
{
	BOOL bRes = false;
	vector<DWORD> res{};

	// 创建进程快照
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hSnapshot == INVALID_HANDLE_VALUE)
	{
		Debug_Error(L"[%s] create snap failed(%d)!!!", __FUNCTION__, GetLastError());
		return bRes;
	}

	do {
		PROCESSENTRY32 pe32;
		pe32.dwSize = sizeof(PROCESSENTRY32);

		if (!Process32First(hSnapshot, &pe32))
		{
			Debug_Error(L"[%s] process first failed!!!", __FUNCTION__);
			break;
		}

		do
		{
			if (StrStrIW(pe32.szExeFile, process) != NULL)
			{
				bRes = TRUE;
				Pids.emplace_back(pe32.th32ProcessID);
			}

		} while (Process32Next(hSnapshot, &pe32));

	} while (FALSE);

	CloseHandle(hSnapshot);

	return bRes;
}

BOOL Process::EnableDebugPrivilege()
{
	BOOL bRet = FALSE;

	HANDLE token;
	TOKEN_PRIVILEGES tp;
	// 打开进程令牌环
	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &token))
	{
		cout << "Open Toekn Failed" << endl;
		return bRet;
	}

	// 获取进程uuid
	LUID luid;
	if (!LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &luid))
	{
		cout << "Get uid failed" << endl;
		return bRet;
	}

	tp.PrivilegeCount = 1;
	tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	tp.Privileges[0].Luid = luid;

	// 调整进程权限
	if (!AdjustTokenPrivileges(token, 0, &tp, sizeof(TOKEN_PRIVILEGES), NULL, NULL))
	{
		cout << "Adjust Privilege failed" << endl;
		return bRet;
	}

	bRet = TRUE;

	return bRet;
}