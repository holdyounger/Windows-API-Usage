// GetCurLoginUser.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
/*
 * main.cpp
 *
 *  Created on: 2021年5月2日
 *      Author: kingfox
 */
#include <iostream>
#include <fstream>
#include <string>
#include <windows.h>
#include <wtsapi32.h>
#include <atlstr.h>
#include <TlHelp32.h>

#pragma comment(lib, "Wtsapi32.lib")

using namespace std;

const char* logfile = "username.log";

string getLoginUsernameByApi()
{
	char username[1024];
	DWORD usernameLength = sizeof username;
	GetUserName(username, &usernameLength);
	return username;
}

void testGetUserNameApi()
{
	ofstream ofs(logfile, ios::app);
	ofs << "GetUserName: " << getLoginUsernameByApi() << endl;
	ofs.close();
}

string getLoginUsernameByEnv()
{
	char username[1024];
	GetEnvironmentVariable("USERNAME", username, sizeof username);
	return username;
}

void testEnvironmentUsername()
{
	ofstream ofs(logfile, ios::app);
	ofs << "ENV_VAR USERNAME: " << getLoginUsernameByEnv() << endl;
	ofs.close();
}


bool Get_LogUser(std::wstring& wsName)
{
	HWND hwnd = ::GetShellWindow();
	if (nullptr == hwnd) {
		return false;
	}

	DWORD dwProcessID = 0;
	GetWindowThreadProcessId(hwnd, &dwProcessID);
	if (0 == dwProcessID) {
		return false;
	}

	HANDLE hProc = NULL;
	HANDLE hToken = NULL;
	TOKEN_USER* pTokenUser = NULL;

	// Open the process with PROCESS_QUERY_INFORMATION access
	hProc = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, dwProcessID);
	if (hProc == NULL)
	{
		return false;
	}
	if (!OpenProcessToken(hProc, TOKEN_QUERY, &hToken))
	{
		return false;
	}

	DWORD dwNeedLen = 0;
	GetTokenInformation(hToken, TokenUser, NULL, 0, &dwNeedLen);
	if (dwNeedLen > 0)
	{
		pTokenUser = (TOKEN_USER*)new BYTE[dwNeedLen];
		if (!GetTokenInformation(hToken, TokenUser, pTokenUser, dwNeedLen, &dwNeedLen))
		{
			return false;
		}
	}
	else
	{
		return false;
	}

	SID_NAME_USE sn;
	WCHAR szDomainName[MAX_PATH];
	DWORD dwDmLen = MAX_PATH;

	WCHAR wstrName[MAX_PATH] = {};
	DWORD nNameLen = MAX_PATH;
	LookupAccountSidW(NULL, pTokenUser->User.Sid, wstrName, &nNameLen,
		szDomainName, &dwDmLen, &sn);

	wsName = wstrName;

	if (hProc)
		::CloseHandle(hProc);
	if (hToken)
		::CloseHandle(hToken);
	if (pTokenUser)
		delete[](char*)pTokenUser;

	return true;
}

BOOL GetLocalCurUserName(std::wstring& strUserName)
{
	BOOL bRet = FALSE;
	DWORD sessionId = WTSGetActiveConsoleSessionId();
	LPWSTR ppBuffer[100];
	DWORD bufferSize;
	bRet = WTSQuerySessionInformationW(WTS_CURRENT_SERVER_HANDLE, sessionId, WTSUserName, ppBuffer, &bufferSize);

	printf("GetLocal1CurUserName --> %ls", *ppBuffer);

	if (bRet == TRUE)
	{
		strUserName = *ppBuffer;
		WTSFreeMemory(ppBuffer);
	}

	return bRet;
}

std::string getLoginUsernameBySession()
{
	LPWSTR usernameBuffer = NULL;
	DWORD infoSize = 0;
	WTSQuerySessionInformationW(WTS_CURRENT_SERVER_HANDLE, WTS_CURRENT_SESSION, WTS_INFO_CLASS::WTSUserName, &usernameBuffer, &infoSize);
	// string username(usernameBuffer);
	CString saUser;
	CStringW saUser1(usernameBuffer);
	saUser.Format("%S", usernameBuffer);
	WTSFreeMemory(usernameBuffer);

	printf("GetUserName:%S\n", saUser1.GetBuffer());

	std::cout << saUser.GetBuffer() << endl;
	std::cout << saUser1.GetBuffer() << endl;
	return "username";
}

DWORD GetPorceeIdByName(const std::string& procName)
{
	const char* FindExe = procName.c_str();
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (INVALID_HANDLE_VALUE == hSnapshot) {
		return NULL;
	}
	PROCESSENTRY32 pe = { sizeof(pe) };
	for (BOOL ret = Process32First(hSnapshot, &pe); ret; ret = Process32Next(hSnapshot, &pe)) {
		if (strcmp(pe.szExeFile, FindExe) == 0) {
			CloseHandle(hSnapshot);
			return pe.th32ProcessID;
		}
		//printf("%-6d %s\n", pe.th32ProcessID, pe.szExeFile);
	}
	CloseHandle(hSnapshot);
}

bool GetUserNameByPid(DWORD pid, std::wstring& username)
{
	DWORD SessionId;
	ProcessIdToSessionId(pid, &SessionId);

	LPWSTR usernameBuffer = NULL;
	DWORD infoSize = 0;
	if (FALSE == WTSQuerySessionInformationW(WTS_CURRENT_SERVER_HANDLE, SessionId, WTS_INFO_CLASS::WTSUserName, &usernameBuffer, &infoSize))
	{
		return false;
	}
	
	username = usernameBuffer;

	return true;
}

void testGetUsernameBySession()
{
	ofstream ofs(logfile, ios::app);
	ofs << "session username: " << getLoginUsernameBySession() << endl;
	ofs << "session username: " << endl;
	ofs.close();
}

int main(int argc, char** argv)
{
	DWORD dwPid;
	wstring wsUserName;
	if (dwPid = GetPorceeIdByName("explorer.exe"))
	{
		if (GetUserNameByPid(dwPid, wsUserName))
		{
			std::wcout << "explorer.exe:" << wsUserName << endl;
		}
	}

	GetLocalCurUserName(wsUserName);
	std::wcout << wsUserName << endl;

	getchar();

	testGetUsernameBySession();
	testGetUserNameApi();
	testEnvironmentUsername();
	
	system("pause");

	return 0;
}