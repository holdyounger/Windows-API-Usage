// GetUSN.cpp : 定义控制台应用程序的入口点。
//

#include <iostream>
#include <Windows.h>

#pragma comment(lib, "Kernel32.lib")

/* *********************
 * CreateIoCompletionPort() 函数有两个功能：
 * 1、创建一个完成端口；
 * 2、将一个设备与一个存在的完成端口进行绑定，在网络编程中这个设备就是一个SOCKET。
 * ********************* */
void getUSN()
{
	HANDLE		fileHd;
	HANDLE		ExistingCompletionPort;
	ULONG_PTR	CompletionKey;
	DWORD		NumberOfConcurrentThreads;
	HANDLE		hRet;

	fileHd = (HANDLE)CreateFileA((LPCSTR)"D:\\1.bat", GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, NULL, NULL);
	if(fileHd == INVALID_HANDLE_VALUE)
	{
		fileHd = NULL;
		MessageBoxA(NULL, "Can not open the file", "Playwav", MB_OK);
	}
	else
	{
		MessageBoxA(NULL, "Open the file", "Playwav", MB_OK);
	}

	hRet = CreateIoCompletionPort(fileHd, ExistingCompletionPort, CompletionKey, NumberOfConcurrentThreads);

	printf("ExistingCompletionPort(%d)\nCompletionKey(%d)\nNumberOfConcurrentThreads(%d)\n", ExistingCompletionPort, CompletionKey, NumberOfConcurrentThreads);
}


int main(int argc, char* argv[])
{
	getUSN();
	return 0;
}

