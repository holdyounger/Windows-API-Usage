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
		// ��ȡ����PID, ����ж�����̣����ȡ���PID
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

	// ͨ�� PID ��ȡ���̱������鿴�Ƿ�Ϊ��Ҫ�Ľ���
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

	// �򿪶�Ӧ�Ľ��̲�ע��
	// ����Ȩ
	if (!Process::EnableDebugPrivilege())
	{
		Debug_Error(L"Enable Debug Privilege Failed");
		return 0;
	}

	// ��Ŀ�����
	HANDLE hTarget = OpenProcess(PROCESS_ALL_ACCESS, false, dwTargetPid);
	if (!hTarget)
	{	
		Debug_Error(L"Open Target Process failed [%d]", GetLastError());
		return 0;
	}

	// ��Ŀ����������ڴ�
	void* pLoadLibFuncParam = nullptr;
	pLoadLibFuncParam = VirtualAllocEx(hTarget, 0, 4096, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	if (pLoadLibFuncParam == nullptr)
	{
		Debug_Error(L"alloc memery failed");
		CloseHandle(hTarget);
		return 0;
	}

	// ��������ڴ���д������
	if (!WriteProcessMemory(hTarget, pLoadLibFuncParam, (LPCVOID)m_strDllPath.GetBuffer(), m_strDllPath.GetLength() + 1, NULL))
	{
		Debug_Error(L"д���ڴ�ʧ��");
		CloseHandle(hTarget);
		return 0;
	}

	// ��ȡ LoadLibraryW �����ĵ�ַ
	HMODULE hNtdll = LoadLibrary(L"kernel32.dll");
	if (!hNtdll)
	{
		Debug_Error(L"����ģ����� %d", GetLastError());
		CloseHandle(hTarget);
		return 0;
	}

	Debug_Run(L"ģ����: 0x%08x", hNtdll);
	void* pLoadLibrary = nullptr;
	pLoadLibrary = GetProcAddress(hNtdll, "LoadLibraryW");
	if (pLoadLibrary == nullptr)
	{
		Debug_Error(L"�Ҳ�������");
		CloseHandle(hTarget);
		return 0;
	}
	Debug_Run(L"������ַ: 0x%08x", pLoadLibrary);


	// ��Ŀ������д����߳� LoadLibraryW 
	DWORD dwThreadId = 0;
	HANDLE hRemoteThread = CreateRemoteThread(hTarget, NULL, 0, (LPTHREAD_START_ROUTINE)pLoadLibrary, (LPVOID)pLoadLibFuncParam, 0, &dwThreadId);
	if (!hRemoteThread)
	{
		Debug_Error(L"��������ʧ��", GetLastError());
		CloseHandle(hTarget);
		return 0;
	}
	Debug_Run(L"���н���", hRemoteThread);

	return 0;
}
