#include <windows.h>
#include <iostream>
#include <tlhelp32.h>

using namespace std;

DWORD GetPidByName(LPCWSTR lpName)
{
	DWORD pid = 0;

	HANDLE hSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (!hSnap)
	{
		cout << "Create Process Snap failed" << endl;
		return 0;
	}

	PROCESSENTRY32 pe;
	pe.dwSize = sizeof(PROCESSENTRY32);
	Process32First(hSnap, &pe);

	do {
		if (!_wcsicmp(lpName, pe.szExeFile))
		{
			return pe.th32ProcessID;
		}
	} while (Process32Next(hSnap, &pe));

	return pid;
}

bool EnableDebugPrivilege()
{
	bool bRet = false;

	HANDLE token;
	TOKEN_PRIVILEGES tp;
	// �򿪽������ƻ�
	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &token))
	{
		cout << "Open Toekn Failed" << endl;
		return bRet;
	}

	// ��ȡ����uuid
	LUID luid;
	if (!LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &luid))
	{
		cout << "Get uid failed" << endl;
		return bRet;
	}

	tp.PrivilegeCount = 1;
	tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	tp.Privileges[0].Luid = luid;

	// ��������Ȩ��
	if (!AdjustTokenPrivileges(token, 0, &tp, sizeof(TOKEN_PRIVILEGES), NULL, NULL))
	{
		cout << "Adjust Privilege failed" << endl;
		return bRet;
	}

	bRet = true;

	return bRet;
}

int main(char* argc, const char* argv[])
{
	// ����Ȩ
	if (!EnableDebugPrivilege())
	{
		cout << "��Ȩʧ��" << endl;
		return 0;
	}

	DWORD dwTargetPid = GetPidByName(L"notepad.exe");
	if (!dwTargetPid)
	{
		cout << "Get Target Process Id failed" << endl;
		return 0;
	}

	// ��Ŀ�����
	HANDLE hTarget = OpenProcess(PROCESS_ALL_ACCESS, false, dwTargetPid);
	if (!hTarget)
	{
		cout << "Open Target Process failed" << endl;
		return 0;
	}

	// ��Ŀ����������ڴ�
	void* pLoadLibFuncParam = nullptr;
	pLoadLibFuncParam = VirtualAllocEx(hTarget, 0, 4096, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	if (pLoadLibFuncParam == nullptr)
	{
		cout << "alloc memery failed" << endl;
		CloseHandle(hTarget);
		return 0;
	}

	LPCTSTR lpParam = L"C:\\6\\SimpleDll.dll";
	if (!WriteProcessMemory(hTarget, pLoadLibFuncParam, (LPCVOID)lpParam, (wcslen(lpParam) + 1) * sizeof(TCHAR), NULL))
	{
		cout << "д���ڴ�ʧ��" << endl;
		CloseHandle(hTarget);
		return 0;
	}
	HMODULE hNtdll = LoadLibrary(L"kernel32.dll");
	if (!hNtdll)
	{
		cout << "����ģ�����" << GetLastError() << endl;
		CloseHandle(hTarget);
		return 0;
	}
	cout << "ģ����: " << hNtdll << endl;
	void* pLoadLibrary = nullptr;
	pLoadLibrary = GetProcAddress(hNtdll, "LoadLibraryW");
	if (pLoadLibrary == nullptr)
	{
		cout << "�Ҳ�������" << endl;
		CloseHandle(hTarget);
		return 0;
	}
	cout << "������ַ: " << pLoadLibrary << endl;
	DWORD dwThreadId = 0;
	HANDLE hRemoteThread = CreateRemoteThread(hTarget, NULL, 0, (LPTHREAD_START_ROUTINE)pLoadLibrary, (LPVOID)pLoadLibFuncParam, 0, &dwThreadId);
	if (!hRemoteThread)
	{
		cout << "��������ʧ��" << GetLastError() << endl;
		CloseHandle(hTarget);
		return 0;
	}
	cout << "���н���" << hRemoteThread << endl;
	getchar();
	getchar();
	CloseHandle(hTarget);
	return 0;
}