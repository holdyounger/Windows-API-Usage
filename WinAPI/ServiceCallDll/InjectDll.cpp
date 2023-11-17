#include "InjectDll.h"
#include "ProcessHelper.h"
#include "define.h"

#ifdef _AMD64_
typedef DWORD(WINAPI* PfnZwCreateThreadEx)(
	PHANDLE ThreadHandle,
	ACCESS_MASK DesiredAccess,
	LPVOID ObjectAttributes,
	HANDLE ProcessHandle,
	LPTHREAD_START_ROUTINE lpStartAddress,
	LPVOID lpParameter,
	ULONG CreateThreadFlags,
	SIZE_T ZeroBits,
	SIZE_T StackSize,
	SIZE_T MaximunStackSize,
	LPVOID pUnkown);


#else

typedef DWORD(WINAPI* PfnZwCreateThreadEx)(
	PHANDLE ThreadHandle,
	ACCESS_MASK DesiredAccess,
	LPVOID ObjectAttributes,
	HANDLE ProcessHandle,
	LPTHREAD_START_ROUTINE lpStartAddress,
	LPVOID lpParameter,
	BOOL CreateThreadFlags,
	DWORD  ZeroBits,
	DWORD  StackSize,
	DWORD  MaximumStackSize,
	LPVOID pUnkown);

#endif // DEBUG

//https://securityxploded.com/ntcreatethreadex.php
//Here is the prototype of NtCreateThreadEx function [undocumented]
typedef NTSTATUS(WINAPI* LPFN_NTCREATETHREADEX)(
	OUT PHANDLE ThreadHandle,
	IN ACCESS_MASK DesiredAccess,
	IN LPVOID ObjectAttributes,
	IN HANDLE ProcessHandle,
	IN LPTHREAD_START_ROUTINE ThreadProcedure,
	IN LPVOID ParameterData,
	IN BOOL CreateSuspended,
	IN SIZE_T StackZeroBits,
	IN SIZE_T SizeOfStackCommit,
	IN SIZE_T SizeOfStackReserve,
	OUT LPVOID BytesBuffer);

typedef HMODULE(WINAPI* LPFN_LOADLIBRARYW)(LPCWSTR lpLibFileName);
typedef HMODULE(WINAPI* LPFN_LOADLIBRARYA)(LPCSTR lpLibFileName);

#ifdef UNICODE
LPFN_LOADLIBRARYW __LoadLibrary = NULL;
#else
LPFN_LOADLIBRARYA __LoadLibrary = NULL;
#endif

InjectDll::InjectDll(CString process, CString DllPath) :
	m_strProcess(process),
	m_strDllPath(DllPath),
	m_type(TYPE_NAME)
{
}

InjectDll::InjectDll(DWORD pid, CString DllPath)
	:m_pid(pid),
	m_strDllPath(DllPath),
	m_type(TYPE_PID) 
{
}

BOOL InjectDll::Inject(INJECT_TYPE injectTpye)
{
	Sleep(20 * 1000);

	vector<DWORD> pids;
	DWORD dwTargetPid = 0;

	if (m_type == TYPE_NAME)
	{
		// 获取进程PID, 如果有多个进程，则获取多个PID
		if (!Process::GetPidsByProcessName(m_strProcess, pids))
		{
			Debug_Error(L"Get Pid List Failed");
			return FALSE;
		}
	}
	else
	{
		pids.emplace_back(m_pid);
	}

#if 0
	// 通过 PID 获取进程变量，查看是否为想要的进程
	if (!m_envName.IsEmpty())
	{
		for (auto pid : pids)
		{
			CString TempValue;
			Process::GetEnvBypid(pid, m_envName, TempValue);

			if (!m_envValue.IsEmpty())
			{
				if (TempValue.CompareNoCase(m_envValue) == 0)
				{
					dwTargetPid = pid;
				}
			}
			else
			{
				if (!TempValue.IsEmpty())
				{
					dwTargetPid = pid;
				}
			}
		}
	}
#endif // 0

	dwTargetPid = m_pid;
	if (dwTargetPid == 0)
	{
		Debug_Error(L"Get Tartget Pid Failed");
		return FALSE;
	}

	Debug_Run(L"Get Target Process Pid Success[%d]", dwTargetPid);

	// 打开对应的进程并注入
	// 先提权
	if (!Process::EnableDebugPrivilege())
	{
		Debug_Error(L"Enable Debug Privilege Failed");
		return 0;
	}

	// 打开目标进程
	HANDLE hTarget = OpenProcess(PROCESS_ALL_ACCESS, false, dwTargetPid);
	if (!hTarget)
	{	
		Debug_Error(L"Open Target Process failed [%d]", GetLastError());
		return 0;
	}

	// 在目标进程申请内存
	void* pLoadLibFuncParam = nullptr;
	pLoadLibFuncParam = VirtualAllocEx(hTarget, 0, 4096, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	if (pLoadLibFuncParam == nullptr)
	{
		Debug_Error(L"alloc memery failed");
		CloseHandle(hTarget);
		return 0;
	}

	// 在申请的内存中写入数据
	if (!WriteProcessMemory(hTarget, pLoadLibFuncParam, (LPCVOID)m_strDllPath.GetBuffer(), m_strDllPath.GetLength() + 1, NULL))
	{
		Debug_Error(L"写入内存失败");
		CloseHandle(hTarget);
		return 0;
	}

	// 获取 LoadLibraryW 函数的地址
	HMODULE hK32dll = LoadLibrary(L"kernel32.dll");
	if (!hK32dll)
	{
		Debug_Error(L"加载模块错误 %d", GetLastError());
		CloseHandle(hTarget);
		return 0;
	}

	HMODULE hNtdll = LoadLibrary(L"ntdll.dll");

	if (!hNtdll)
	{
		Debug_Error(L"[%s-%d]ntdll Load Error %d", __FUNCTIONW__, __LINE__, GetLastError());
		CloseHandle(hTarget);
		return 0;
	}

	Debug_Run(L"模块句柄: 0x%08x", hK32dll);

	// 获取 LoadLibrary 函数的地址
#ifdef UNICODE
	__LoadLibrary = (LPFN_LOADLIBRARYW)GetProcAddress(hK32dll, "LoadLibraryW");
#else
	pLoadLibrary = (LPFN_LOADLIBRARYW)GetProcAddress(hK32dll, "LoadLibraryA");
#endif
	if (__LoadLibrary == nullptr)
	{
		Debug_Error(L"找不到函数");
		CloseHandle(hTarget);
		return 0;
	}
	Debug_Run(L"函数地址: 0x%08x", __LoadLibrary);


	HANDLE hRemoteThread = NULL;
	if (injectTpye == INJECT_REMOTE_THREAD)
	{
		// 在目标进程中创建线程 LoadLibraryW 
		DWORD dwThreadId = 0;
		hRemoteThread = CreateRemoteThread(hTarget, NULL, 0, (LPTHREAD_START_ROUTINE)__LoadLibrary, (LPVOID)pLoadLibFuncParam, 0, &dwThreadId);
	}
	else if (injectTpye == INJECT_NT_CREATETH)
	{
		BOOL  IsOk = FALSE;


		LPFN_NTCREATETHREADEX __NtCreateThreadEx = NULL;
		__NtCreateThreadEx = (LPFN_NTCREATETHREADEX)GetProcAddress(hNtdll,
			"NtCreateThreadEx");
		if (__NtCreateThreadEx == NULL)
		{
			Debug_Error(L"[%s] Get NtCreateThread Failed %d", __FUNCTIONW__, GetLastError());

			CloseHandle(hTarget);
			return 0;
		}

		IsOk = __NtCreateThreadEx(&hRemoteThread,
			THREAD_ALL_ACCESS, NULL, hTarget, (LPTHREAD_START_ROUTINE)__LoadLibrary, pLoadLibFuncParam,
			FALSE, NULL, NULL, NULL, NULL);
		if (IsOk < 0)
		{
			Debug_Error(L"[%s] NtCreateThread Failed %d", __FUNCTIONW__, GetLastError());
			VirtualFreeEx(hTarget, pLoadLibFuncParam, m_strDllPath.GetLength(), MEM_RELEASE);
			CloseHandle(hTarget);
			return 0;
		}

		else
		{
			WaitForSingleObject(hRemoteThread, INFINITE);
			VirtualFreeEx(hTarget, pLoadLibFuncParam, m_strDllPath.GetLength(), MEM_RELEASE);
			CloseHandle(hTarget);
		}
	}
	else
	{
		// 获取 ZwCreateThreadEx 函数的地址
		HMODULE hNtdll = LoadLibrary(L"ntdll.dll");
		if (!hNtdll)
		{
			Debug_Error(L"[%s-%d]ntdll Load Error %d", __FUNCTIONW__ , __LINE__, GetLastError());
			CloseHandle(hTarget);
			return 0;
		}

		PfnZwCreateThreadEx pFnZwCreateThreadEx = NULL;
		pFnZwCreateThreadEx = reinterpret_cast<PfnZwCreateThreadEx>(GetProcAddress(hNtdll, "ZwCreateThreadEx"));
		if (NULL == pFnZwCreateThreadEx)
		{
			CloseHandle(hTarget);
			Debug_Error(L"[%s-%d]ZwCreateThreadEx Failed %d", __FUNCTIONW__, __LINE__, GetLastError());
			return 0;
		}

		pFnZwCreateThreadEx(&hRemoteThread, PROCESS_ALL_ACCESS, NULL, hTarget, (LPTHREAD_START_ROUTINE)__LoadLibrary, pLoadLibFuncParam, 0, 0, 0, 0, NULL);
		WaitForSingleObject(hTarget, 2000);
		if (NULL == hRemoteThread)
		{
			Debug_Error(L"[%s-%d]ZwCreateThreadEx Call Failed %d", __FUNCTIONW__, __LINE__, GetLastError());
			return 0;
		}
	}

	if (!hRemoteThread)
	{
		Debug_Error(L"CreateRemoteThread Failed: %d", GetLastError());
		CloseHandle(hTarget);
		return 0;
	}
	Debug_Run(L"运行结束", hRemoteThread);

	return 0;
}