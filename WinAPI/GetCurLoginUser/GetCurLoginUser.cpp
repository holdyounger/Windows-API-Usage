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

std::string getLoginUsernameBySession()
{
	char* usernameBuffer = nullptr;
	DWORD infoSize = 0;
	WTSQuerySessionInformation(WTS_CURRENT_SERVER_HANDLE, WTS_CURRENT_SESSION, WTS_INFO_CLASS::WTSUserName, &usernameBuffer, &infoSize);
	string username(usernameBuffer);
	WTSFreeMemory(usernameBuffer);
	return username;
}

void testGetUsernameBySession()
{
	ofstream ofs(logfile, ios::app);
	ofs << "session username: " << getLoginUsernameBySession() << endl;
	ofs.close();
}

int main(int argc, char** argv)
{
	ofstream ofs(logfile);
	testGetUserNameApi();
	testEnvironmentUsername();
	testGetUsernameBySession();
	return 0;
}