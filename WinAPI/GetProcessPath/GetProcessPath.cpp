// GetProcessPath.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <windows.h>
#include <tlhelp32.h>	//进程快照函数头文件
#include <Psapi.h>
#include <stdio.h>
#include <string>
#include <atlstr.h>
#include <locale>
#include <algorithm>
#include <codecvt>

#pragma comment (lib,"Psapi.lib")

#define REG_UNINST_KEY		 _T("Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\TrustAgent")
#define REG_TRUSTAPP_PATH	 _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\App Paths\\TrustAgent.exe")

using namespace std;

/*
* @fn           NSPTRUSTTOOLS::GetInstalledPath
* @brief        查询注册表获取零信任客户端程序位置
* @param[in]	
	bUsedWithTrustSelf 是否查询零信任的注册表 true: 查询零信任本身的注册表 false：查询天擎目录下零信任注册表
* @param[out]
* @return       string: 返回查询的路径，如果查询失败返回空
*
* @detail
*/
std::string GetInstalledPath(bool bUsedWithTrustSelf = false)
{
	USES_CONVERSION;
	BOOL bRet = FALSE;

	HKEY hKey;
	CString strCov;
	std::string strExePath = "";
	TCHAR szLocation[MAX_PATH] = { '\0' };
	DWORD dwSize = sizeof(DWORD);
	DWORD dwIndex = 0;
	DWORD dwType = REG_SZ;
	LONG lRet = RegOpenKeyEx(HKEY_LOCAL_MACHINE, bUsedWithTrustSelf ? REG_UNINST_KEY : REG_TRUSTAPP_PATH, 0, KEY_READ, &hKey);

	if (ERROR_SUCCESS != lRet)
	{
		goto _END_;
	}

	if (bUsedWithTrustSelf)
	{
		lRet = RegQueryValueEx(hKey, TEXT("InstallDir"), 0, &dwType, NULL, &dwSize);
		lRet = RegQueryValueEx(hKey, TEXT("InstallDir"), NULL, &dwType, (LPBYTE)&szLocation, &dwSize);
	}
	else
	{
		lRet = RegQueryValueEx(hKey, TEXT(""), 0, &dwType, NULL, &dwSize);
		lRet = RegQueryValueEx(hKey, TEXT(""), NULL, &dwType, (LPBYTE)&szLocation, &dwSize);
	}
	cout << "RegQueryValueEx returns " << lRet << "dwSize = " << dwSize << endl;

	if (ERROR_SUCCESS != lRet)
	{
		goto _END_;
	}

	strCov.Format(_T("%s"), szLocation);
	if (wcslen(szLocation))
	{
		strExePath = W2A(strCov.GetBuffer());
		if (bUsedWithTrustSelf)
		{
			strExePath.append("\\").append("TrustAgent.exe");
		}

		// cout << " Location: " << strExePath.c_str() << endl;
		strCov.ReleaseBuffer();
	}
	else
	{
		cout << " GetLastError()= " << GetLastError();
	}

_END_:
	if (hKey)
	{
		RegCloseKey(hKey);
		hKey = nullptr;
	}

	return strExePath;
}

BOOL GetProcessFullPath(DWORD dwPID, TCHAR pszFullPath[MAX_PATH], __out std::string& str)
{
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPID);
	if (!hProcess)
	{
		// cout << "Query Process With PID Failed, GetLastError()=" << GetLastError() << " PID:" << dwPID << endl;
		str = "";
	}
	else
	{
		cout << "[openPID]" << dwPID;
	}

	char filePath[MAX_PATH];
	DWORD ret = GetModuleFileNameExA(hProcess, NULL, filePath, MAX_PATH);
	std::string file = filePath;

	if (hProcess)
	{
		CloseHandle(hProcess);
		hProcess = NULL;
	}

	str = ret == 0 ? "" : file;
#if 0
	cout << " [Path]" << str;
#endif

	return ret != 0;
}

BOOL IsExistTrustAgentPro()
{
	int countProcess = 0;	//当前进程数量计数变量
	HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0); // 创建进程快照
	PROCESSENTRY32W currentProcess;	// 用来接收 hProcessSnap 的信息

	std::string strPath = GetInstalledPath(true); // 安装路径

	strPath = "explorer.exe";
	if (strPath.empty())
	{
		cout << "strpath empty" << endl;
		// return false;
	}
	cout << strPath << endl;

	std::transform(strPath.begin(), strPath.end(), strPath.begin(), ::tolower);

	currentProcess.dwSize = sizeof(currentProcess);		//在使用这个结构之前，先设置它的大小
	HANDLE hProcess = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);//给系统内的所有进程拍一个快照

	if (hProcess == INVALID_HANDLE_VALUE)
	{
		printf("CreateToolhelp32Snapshot()调用失败!\n");
		return -1;
	}

	int i = 0;
	bool bMore = Process32First(hProcessSnap, &currentProcess);	//获取第一个进程信息
	while (bMore) {
		std::string str;
		TCHAR szProcessName[MAX_PATH] = { 0 };
		cout << "[" /*<< strPath << " "*/ << i++ << "] ";
		bool bRet = GetProcessFullPath(currentProcess.th32ProcessID, szProcessName, str);
		
		// cout << " [compare]:" << strPath.compare(str) << endl;
		std::transform(str.begin(), str.end(), str.begin(), ::tolower);

#ifdef _DEBUG
		if ( 0 == strPath.compare(str) )
		{
			cout << "[" << "!!!" << strPath << i++ << "] " << "[str] " << str;
			return TRUE;
		}
		else
		{
			cout << "[" << strPath << i++ << "] " << str;
		}
#else
		if (bRet) cout << " [str] " << str << endl;
		else cout << endl;
#endif // _DEBUG

		bMore = Process32Next(hProcessSnap, &currentProcess);	//遍历下一个
		countProcess++;
	}

	CloseHandle(hProcess);	//清除hProcess句柄

	return FALSE;
}
int main()
{
	bool bFound = IsExistTrustAgentPro();
    std::cout << "IsExistTrustAgentPro:" << bFound;
}