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

	bRet = true;

	return bRet;
}

int main(char* argc, const char* argv[])
{
	// 先提权
	if (!EnableDebugPrivilege())
	{
		cout << "提权失败" << endl;
		return 0;
	}

	DWORD dwTargetPid = GetPidByName(L"notepad.exe");
	if (!dwTargetPid)
	{
		cout << "Get Target Process Id failed" << endl;
		return 0;
	}

	// 打开目标进程
	HANDLE hTarget = OpenProcess(PROCESS_ALL_ACCESS, false, dwTargetPid);
	if (!hTarget)
	{
		cout << "Open Target Process failed" << endl;
		return 0;
	}

	// 在目标进程申请内存
	void* pLoadLibFuncParam = nullptr;
	pLoadLibFuncParam = VirtualAllocEx(hTarget, 0, 4096, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	if (pLoadLibFuncParam == nullptr)
	{
		cout << "alloc memery failed" << endl;
		CloseHandle(hTarget);
		return 0;
	}

	// 在申请的内存中写入数据
	LPCTSTR lpParam = L"C:\\6\\SimpleDll.dll";
	if (!WriteProcessMemory(hTarget, pLoadLibFuncParam, (LPCVOID)lpParam, (wcslen(lpParam) + 1) * sizeof(TCHAR), NULL))
	{
		cout << "写入内存失败" << endl;
		CloseHandle(hTarget);
		return 0;
	}

	// 获取 LoadLibraryW 函数的地址
	HMODULE hNtdll = LoadLibrary(L"kernel32.dll");
	if (!hNtdll)
	{
		cout << "加载模块错误" << GetLastError() << endl;
		CloseHandle(hTarget);
		return 0;
	}
	cout << "模块句柄: " << hNtdll << endl;
	void* pLoadLibrary = nullptr;
	pLoadLibrary = GetProcAddress(hNtdll, "LoadLibraryW");
	if (pLoadLibrary == nullptr)
	{
		cout << "找不到函数" << endl;
		CloseHandle(hTarget);
		return 0;
	}
	cout << "函数地址: " << pLoadLibrary << endl;

	// 在目标进程中创建线程 LoadLibraryW 
	DWORD dwThreadId = 0;
	HANDLE hRemoteThread = CreateRemoteThread(hTarget, NULL, 0, (LPTHREAD_START_ROUTINE)pLoadLibrary, (LPVOID)pLoadLibFuncParam, 0, &dwThreadId);
	if (!hRemoteThread)
	{
		cout << "创建进程失败" << GetLastError() << endl;
		CloseHandle(hTarget);
		return 0;
	}
	cout << "运行结束" << hRemoteThread << endl;
	getchar();
	getchar();
	CloseHandle(hTarget);
	return 0;
}