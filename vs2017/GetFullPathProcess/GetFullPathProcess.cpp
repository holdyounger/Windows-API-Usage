// GetFullPathProcess.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <windows.h>
#include <tlhelp32.h>	//进程快照函数头文件
#include <stdio.h>
#include <string>

#include <tchar.h>
#include <Psapi.h>
#pragma comment (lib,"Psapi.lib")

using namespace std;
BOOL DosPathToNtPath(LPTSTR pszDosPath, LPTSTR pszNtPath)
{
	TCHAR			szDriveStr[500];
	TCHAR			szDrive[3];
	TCHAR			szDevName[100];
	INT				cchDevName;
	INT				i;

	//检查参数
	if (!pszDosPath || !pszNtPath)
		return FALSE;

	//获取本地磁盘字符串
	if (GetLogicalDriveStrings(sizeof(szDriveStr), szDriveStr))
	{
		for (i = 0; szDriveStr[i]; i += 4)
		{
			if (!lstrcmpi(&(szDriveStr[i]), _T("A:\\")) || !lstrcmpi(&(szDriveStr[i]), _T("B:\\")))
				continue;

			szDrive[0] = szDriveStr[i];
			szDrive[1] = szDriveStr[i + 1];
			szDrive[2] = '\0';
			if (!QueryDosDevice(szDrive, szDevName, 100))//查询 Dos 设备名
				return FALSE;

			cchDevName = lstrlen(szDevName);
			if (_tcsnicmp(pszDosPath, szDevName, cchDevName) == 0)//命中
			{
				lstrcpy(pszNtPath, szDrive);//复制驱动器
				lstrcat(pszNtPath, pszDosPath + cchDevName);//复制路径

				return TRUE;
			}
		}
	}

	lstrcpy(pszNtPath, pszDosPath);

	return FALSE;
}

//获取进程完整路径
BOOL GetProcessFullPath(DWORD dwPID, TCHAR pszFullPath[MAX_PATH])
{
	TCHAR		szImagePath[MAX_PATH];
	HANDLE		hProcess;
	if (!pszFullPath)
		return FALSE;

	pszFullPath[0] = '\0';
	hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, 0, dwPID);
	if (!hProcess)
	{
		return FALSE;
	}	

	if (!GetProcessImageFileName(hProcess, szImagePath, MAX_PATH))
	{
		CloseHandle(hProcess);
		return FALSE;
	}

	if (!DosPathToNtPath(szImagePath, pszFullPath))
	{
		CloseHandle(hProcess);
		return FALSE;
	}

	CloseHandle(hProcess);

	_tprintf(_T("PID = %d, %s \r\n"), dwPID, pszFullPath);
	return TRUE;
}

string GetProcessInfo(__in HANDLE hProcess, __in WCHAR* processName)
{
	PROCESSENTRY32* pinfo = new PROCESSENTRY32; //进程信息 （pinfo->dwSize = sizeof(PROCESSENTRY32);）
	MODULEENTRY32* minfo = new MODULEENTRY32; //模块信息 （minfo->dwSize = sizeof(MODULEENTRY32);）
	char shortpath[MAX_PATH];				//保存路径变量

	int flag = Process32First(hProcess, pinfo);	// 从第一个进程开始
	while (flag) {

		if (wcscmp(pinfo->szExeFile, processName) == 0) {

			// 创建进程快照
			HANDLE hModule = CreateToolhelp32Snapshot(
				TH32CS_SNAPMODULE,		//（DWORD） 快照返回的对象，TH32CS_SNAPMODULE 是指 "特定进程的使用模块的列表"
				pinfo->th32ProcessID	//（DWORD） 要获取快照进程的PID，当前进程/系统列表 快照时设为0
			);

			// 把第一个模块信息给 minfo
			Module32First(
				hModule,  //（HANDLE） CreateToolhelp32Snapshot 的返回句柄
				minfo	 // （LPMODULEENTRY32）  接收模块信息
			);

			// 把文件路径给 shortpath
			GetShortPathName(
				minfo->szExePath,	//  文件路径（但最好不要用这个，因为这个碰到中文会出现乱码）
				(LPWSTR)shortpath,		// 用来接收 minfo->szExePath 兼容中文的值
				256			// 缓冲区大小
			);

			return shortpath;
		}

		// 下一个进程
		flag = Process32Next(hProcess, pinfo);
	}

	return NULL;
}

int getAllProcess()
{
	int countProcess = 0;	//当前进程数量计数变量
	HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0); // 创建进程快照
	PROCESSENTRY32W currentProcess;	// 用来接收 hProcessSnap 的信息

	currentProcess.dwSize = sizeof(currentProcess);		//在使用这个结构之前，先设置它的大小
	HANDLE hProcess = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);//给系统内的所有进程拍一个快照

	if (hProcess == INVALID_HANDLE_VALUE)
	{
		printf("CreateToolhelp32Snapshot()调用失败!\n");
		return -1;
	}

	bool bMore = Process32First(hProcessSnap, &currentProcess);	//获取第一个进程信息
	while (bMore) {
		TCHAR szProcessName[MAX_PATH] = { 0 };
		GetProcessFullPath(currentProcess.th32ProcessID, szProcessName);
		bMore = Process32Next(hProcessSnap, &currentProcess);	//遍历下一个	
		countProcess++;
	}

	CloseHandle(hProcess);	//清除hProcess句柄
	printf("共有以上%d个进程在运行\n", countProcess);
}

int main()
{
	getAllProcess();
	return 0;
}


// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
