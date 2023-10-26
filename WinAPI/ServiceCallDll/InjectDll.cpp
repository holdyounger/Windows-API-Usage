#include "InjectDll.h"
#include "ProcessHelper.h"
#include "define.h"

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

BOOL InjectDll::Inject()
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

	// 通过 PID 获取进程变量，查看是否为想要的进程
	if (!m_envName.IsEmpty())
	{
		for (auto pid : pids)
		{
			CString TempValue;
			Process::GetEnvBypid(pid, m_envName, TempValue);

			if(!m_envValue.IsEmpty())
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
	HMODULE hNtdll = LoadLibrary(L"kernel32.dll");
	if (!hNtdll)
	{
		Debug_Error(L"加载模块错误 %d", GetLastError());
		CloseHandle(hTarget);
		return 0;
	}

	Debug_Run(L"模块句柄: 0x%08x", hNtdll);
	void* pLoadLibrary = nullptr;
	pLoadLibrary = GetProcAddress(hNtdll, "LoadLibraryW");
	if (pLoadLibrary == nullptr)
	{
		Debug_Error(L"找不到函数");
		CloseHandle(hTarget);
		return 0;
	}
	Debug_Run(L"函数地址: 0x%08x", pLoadLibrary);


	// 在目标进程中创建线程 LoadLibraryW 
	DWORD dwThreadId = 0;
	HANDLE hRemoteThread = CreateRemoteThread(hTarget, NULL, 0, (LPTHREAD_START_ROUTINE)pLoadLibrary, (LPVOID)pLoadLibFuncParam, 0, &dwThreadId);
	if (!hRemoteThread)
	{
		Debug_Error(L"创建进程失败", GetLastError());
		CloseHandle(hTarget);
		return 0;
	}
	Debug_Run(L"运行结束", hRemoteThread);

	return 0;
}
