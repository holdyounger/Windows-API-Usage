// LookupPersistentTcpPortReservation.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#ifndef UNICODE
#define UNICODE
#endif

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <Windows.h.>
#include <winsock2.h>
#include <mstcpip.h>
#include <ws2ipdef.h>
#include <iphlpapi.h>
#include <stdio.h>
#include <stdlib.h>
#include <strsafe.h>  

// Need to link with iphlpapi.lib
#pragma comment(lib, "iphlpapi.lib")

// Need to link with Ws2_32.lib for Winsock functions
#pragma comment(lib, "ws2_32.lib")


void ErrorExit(LPTSTR lpszFunction)
{
	// Retrieve the system error message for the last-error code  

	LPVOID lpMsgBuf;
	LPVOID lpDisplayBuf;
	DWORD dw = GetLastError();

	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		dw,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf,
		0, NULL);

	// Display the error message and exit the process  

	lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT,
		(lstrlen((LPCTSTR)lpMsgBuf) + lstrlen((LPCTSTR)lpszFunction) + 40) * sizeof(TCHAR));
	StringCchPrintf((LPTSTR)lpDisplayBuf,
		LocalSize(lpDisplayBuf),
		TEXT("%s failed with error %d: %s"),
		lpszFunction, dw, lpMsgBuf);
	MessageBox(NULL, (LPCTSTR)lpDisplayBuf, TEXT("Error"), MB_OK);

	LocalFree(lpMsgBuf);
	LocalFree(lpDisplayBuf);
	ExitProcess(dw);
}

int wmain(int argc, WCHAR **argv) {

	// Declare and initialize variables
	int startPort = 0;         // host byte order
	int numPorts = 0;
	USHORT startPortns = 0;    // Network byte order
	ULONG64 resToken = { 0 };

	unsigned long status = 0;

	WSADATA wsaData = { 0 };
	int iResult = 0;

	SOCKET sock = INVALID_SOCKET;
	int iFamily = AF_INET;
	int iType = SOCK_STREAM;
	int iProtocol = IPPROTO_TCP;

	DWORD bytesReturned = 0;

	// Note that the sockaddr_in struct works only with AF_INET not AF_INET6
	// An application needs to use the sockaddr_in6 for AF_INET6
	sockaddr_in service;
	sockaddr_in sockName;
	int nameLen = sizeof(sockName);

	// Validate the parameters
	if (argc != 3) {
		wprintf(L"usage: %s <Starting Port> <Number of Ports>\n",
			argv[0]);
		wprintf(L"Look up a persistent TCP port reservation\n");
		wprintf(L"Example usage:\n");
		wprintf(L"   %s 5000 20\n", argv[0]);
		wprintf(L"   where StartPort=5000 NumPorts=20");
		return 1;
	}

	startPort = _wtoi(argv[1]);
	if (startPort < 0 || startPort> 65535) {
		wprintf(L"Starting point must be either 0 or between 1 and 65,535\n");
		return 1;
	}
	startPortns = htons((USHORT)startPort);

	numPorts = _wtoi(argv[2]);
	if (numPorts < 0) {
		wprintf(L"Number of ports must be a positive number\n");
		return 1;
	}

	status = LookupPersistentTcpPortReservation((USHORT)startPortns, (USHORT)numPorts, &resToken);
	if (status != NO_ERROR)
	{
		wprintf(L"LookupPersistentTcpPortReservation returned error: %ld GetLastError(%d)\n",
			status, GetLastError());
		ErrorExit(TEXT("LookupPersistentTcpPortReservation"));

		return 1;
	}

	wprintf(L"LookupPersistentTcpPortReservation call succeeded\n");
	wprintf(L"  Token = %I64d\n", resToken);

	// Comment out this block if you don't want to create a socket and associate it with the 
	// persistent reservation

	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		wprintf(L"WSAStartup failed with error = %d\n", iResult);
		return 1;
	}

	sock = socket(iFamily, iType, iProtocol);
	if (sock == INVALID_SOCKET)
		wprintf(L"socket function failed with error = %d\n", WSAGetLastError());
	else {
		wprintf(L"socket function succeeded\n");

		iResult =
			WSAIoctl(sock, SIO_ASSOCIATE_PORT_RESERVATION, (LPVOID)& resToken,
				sizeof(ULONG64), NULL, 0, &bytesReturned, NULL, NULL);
		if (iResult != 0) {
			wprintf
			(L"WSAIoctl(SIO_ASSOCIATE_PORT_RESERVATION) failed with error = %d\n",
				WSAGetLastError());
		}
		else {
			wprintf(L"WSAIoctl(SIO_ASSOCIATE_PORT_RESERVATION) succeeded, bytesReturned = %u\n",
				bytesReturned);

			service.sin_family = AF_INET;
			service.sin_addr.s_addr = INADDR_ANY;
			service.sin_port = 0;

			iResult = bind(sock, (SOCKADDR*)&service, sizeof(service));
			if (iResult == SOCKET_ERROR)
				wprintf(L"bind failed with error = %d\n", WSAGetLastError());
			else {
				wprintf(L"bind succeeded\n");
				iResult = getsockname(sock, (SOCKADDR*)&sockName, &nameLen);
				if (iResult == SOCKET_ERROR)
					wprintf(L"getsockname failed with error = %d\n", WSAGetLastError());
				else {
					wprintf(L"getsockname succeeded\n");
					wprintf(L"Port number allocated = %u\n", ntohs(sockName.sin_port));
				}
			}
		}

		if (sock != INVALID_SOCKET) {
			iResult = closesocket(sock);
			if (iResult == SOCKET_ERROR) {
				wprintf(L"closesocket failed with error = %d\n", WSAGetLastError());
			}
		}
	}
	WSACleanup();

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
