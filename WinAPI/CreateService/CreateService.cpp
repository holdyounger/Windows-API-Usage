// CreateService.cpp : 定义控制台应用程序的入口点。
//

// #include "stdafx.h"
#include <Windows.h>
#include <stdio.h>
#include <WinSvc.h>
#include <winioctl.h>
#include <tchar.h>
#pragma comment(lib, "Advapi32.lib")


BOOL trustdservice_Install();
BOOL trustdservice_Uninstall();

int g_dwLanguage = 1;

int _tmain(int argc, _TCHAR* argv[])
{
	printf("-------------Start Create-------------\n");
	trustdservice_Install();
	printf("-------------Start Created SUCCESS-------------\n");
	return 0;
}


BOOL trustdservice_Install()
{
	BOOL bResult = FALSE;
	//打开服务控制管理器
	SC_HANDLE hSCM = ::OpenSCManager(NULL, NULL, /*SC_MANAGER_ALL_ACCESS*/SC_MANAGER_CONNECT | SC_MANAGER_CREATE_SERVICE);
	if (hSCM == NULL)
	{
		printf(("Couldn't open service manager trustdservice, 0x%x\n"), GetLastError());
		return bResult;
	}

	// Get the executable file path
	// TCHAR szFilePath[MAX_PATH] = { _T("system32\\drivers\\trustdservice.sys") };
	TCHAR szFilePath[MAX_PATH] = { _T("C:\\Program Files (x86)\QLink\\TrustAgent\\trustservice.exe") };

	LPCTSTR lpszServiceName = _T("trustservice");
	LPCTSTR lpszDisplayName = _T("trustservice");
#if 0
	DWORD	dwServiceType = SERVICE_FILE_SYSTEM_DRIVER;		// 运行于独立进程的服务程序
	DWORD	dwStartType = SERVICE_DEMAND_START;				// 由服务控制管理器(SCM)启动的服务
	DWORD	dwErrorControl = SERVICE_ERROR_NORMAL;			// 服务启动程序将把该错误记录到事件日志中并返回继续执行
#endif

	DWORD	dwServiceType = SERVICE_WIN32_OWN_PROCESS;		// 运行于独立进程的服务程序
	DWORD	dwStartType = SERVICE_AUTO_START;				// 由服务控制管理器(SCM)启动的服务
	DWORD	dwErrorControl = SERVICE_ERROR_NORMAL;			// 服务启动程序将把该错误记录到事件日志中并返回继续执行

	DWORD	dwOrder = 0;

	//创建服务
	SC_HANDLE hService = ::CreateService(hSCM,
		lpszServiceName,
		lpszDisplayName,
		SERVICE_ALL_ACCESS,
		dwServiceType,
		dwStartType,
		dwErrorControl,
		szFilePath,
		_T("FSFilter Activity Monitor"),
		&dwOrder,
		_T("FltMgr"),
		NULL,
		NULL
	);

	if (hService == NULL)
	{
		::CloseServiceHandle(hSCM);

		if (ERROR_SERVICE_EXISTS != ::GetLastError())
		{
			printf(("Couldn't create service manager trustdservice, errno:%d\n"), GetLastError());


			if (1 == g_dwLanguage)
			{
				MessageBoxEx(NULL, _T("Failed to create a service, restart the operating system and try again! "), _T("trustdservice"), MB_OK, MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US));
			}
			else
			{

				MessageBoxEx(NULL, _T("创建服务失败,重启操作系统后重试！"), _T("trustdservice"), MB_OK, MAKELANGID(LANG_CHINESE_SIMPLIFIED, SUBLANG_CHINESE_SIMPLIFIED));
			}


			return bResult;
		}
		else
		{
			bResult = TRUE;
			printf(("trustdservice is exist, Couldn't create service manager\n"));
			return bResult;
		}

		return bResult;
	}
	else
	{
		/*--这个函数有什么意义？
		if (!::ChangeServiceConfig(
			hService,
			dwServiceType,
			dwStartType,
			dwErrorControl,
			szFilePath,
			_T("FSFilter Activity Monitor"),
			&dwOrder,
			_T(""),
			NULL,
			NULL,
			lpszDisplayName))
		{
			WriteError(_T("ChangeServiceConfig trustdservice Error:%d"), GetLastError());
		}
		*/

		//创建minifilter专属注册表项，即Instance
		HKEY	hKey = NULL;
		DWORD	dwData = 0;
		WCHAR	wszTemp[MAX_PATH] = { 0 };

		#if 0
swprintf(wszTemp, MAX_PATH, L"SYSTEM\\CurrentControlSet\\Services\\%ws\\Instances", lpszServiceName);
		if (RegCreateKeyEx(HKEY_LOCAL_MACHINE, wszTemp, 0, LPWSTR(""), REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, (LPDWORD)&dwData) != ERROR_SUCCESS)
		{
			printf(("trustdservice  Create Instance Failed, 0x%x\n"), GetLastError());
			goto DONE;
		}
		else
		{
			printf(("trustdservice  Create Instance SUCCESS\n"));
		}

		//暂定默认Instance和服务同名
		if (RegSetValueEx(hKey, L"DefaultInstance", 0, REG_SZ, (CONST BYTE*)lpszServiceName, (DWORD)wcslen(lpszServiceName) * sizeof(WCHAR)) != ERROR_SUCCESS)
		{
			printf(("trustdservice  Create DefaultInst  Failed, 0x%x\n"), GetLastError());
			RegCloseKey(hKey);
			goto DONE;
		}
		else
		{
			printf(("trustdservice  Create DefaultInst SUCCESS\n"));
		}

		RegFlushKey(hKey);
		RegCloseKey(hKey);

		//添加Altitude和flag

		swprintf(wszTemp, MAX_PATH, L"%ws\\%ws", wszTemp, lpszServiceName);

		if (RegCreateKeyExW(HKEY_LOCAL_MACHINE, wszTemp, 0, LPWSTR(""), REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, (LPDWORD)&dwData) != ERROR_SUCCESS)
		{
			printf(("trustdservice  Create Instance subkey  Failed, 0x%x\n"), GetLastError());
			goto DONE;
		}
		else
		{
			printf(("trustdservice  Create Instance subkey SUCCESS\n"));
		}

		// 注册表驱动程序的Altitude 值
		PWCHAR pwszAltitude = LPWSTR("368329");  //这个值需要尽快申请

		if (RegSetValueEx(hKey, L"Altitude", 0, REG_SZ, (CONST BYTE*)pwszAltitude, (DWORD)wcslen(pwszAltitude) * sizeof(WCHAR)) != ERROR_SUCCESS)
		{
			RegCloseKey(hKey);
			goto DONE;
		}
		else
		{
			printf(("trustdservice  set Instance subkey [Altitude] SUCCESS\n"));
		}

		// 注册表驱动程序的Flags 值
		dwData = 0x0;
		if (RegSetValueEx(hKey, L"Flags", 0, REG_DWORD, (CONST BYTE*) & dwData, sizeof(DWORD)) != ERROR_SUCCESS)
		{
			RegCloseKey(hKey);
			goto DONE;
		}
		else
		{
			printf(("trustdservice  set Instance subkey [Flags] SUCCESS\n"));
		}

		//everything is ok !

		RegFlushKey(hKey);
		RegCloseKey(hKey);
#endif

		bResult = TRUE;


	}

DONE:
	::CloseServiceHandle(hService);
	::CloseServiceHandle(hSCM);

	return bResult;
}


BOOL trustdservice_Uninstall() {
	SC_HANDLE hSCManager;
	SC_HANDLE hSCService;
	SERVICE_STATUS curStatus;
	SERVICE_STATUS ctrlstatus;

	LPCTSTR lpszServiceName = _T("trustservice");

	hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (hSCManager == NULL) {
		return FALSE;
	}
	hSCService = OpenService(hSCManager, lpszServiceName, SERVICE_ALL_ACCESS);
	if (hSCService == NULL) {
		return FALSE;
	}
	if (!QueryServiceStatus(hSCService, &curStatus)) {
		return FALSE;
	}

	if (curStatus.dwCurrentState != SERVICE_STOPPED) {
		if (!ControlService(hSCService, SERVICE_CONTROL_STOP, &ctrlstatus)) {
			printf("Stop Service failed:%d!\n", GetLastError());
			return FALSE;
		}
	}

	if (DeleteService(hSCService)) {
		printf("Uninstall Service Success!\n");
	}
	else {
		printf("Uninstall Service failed:%d!\n", GetLastError());
	}

	CloseServiceHandle(hSCService);
	CloseServiceHandle(hSCManager);

	printf("Uninstall Service Success!\n");

	return TRUE;
}
