// Client.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <WinSock2.h>
#include <stdio.h>
#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "ws2_32.lib")
int main()
{
	DWORD dwRet;
	SOCKET m_Socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	SOCKADDR_IN addrServer;
	SOCKADDR_IN myaddr;
	unsigned long ul = 1;
	int iBindRet;
	DWORD dwResult;
	int nPort;

	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);

	if (m_Socket != INVALID_SOCKET)
	{
		closesocket(m_Socket);
		m_Socket = INVALID_SOCKET;
	}

	m_Socket = socket(AF_INET, SOCK_STREAM, 0);
	if (m_Socket == INVALID_SOCKET)
	{
		printf("LINE [%d] ,CreateTCPConnection socket() called failed! GetLastError=%d\n", __LINE__, WSAGetLastError());
		dwRet = WSAGetLastError();
		goto END;
	}

	/*
	printf("请输入端口：");
	scanf("%d", &nPort);
	myaddr.sin_port = htons(nPort);  //系统随机选择一个未被使用的端口号 
	myaddr.sin_addr.s_addr = INADDR_ANY; // 填入本机IP地址 
	// myaddr.sin_addr.s_addr = inet_addr("192.168.3.171"); // 填入本机IP地址 
	myaddr.sin_family = AF_INET;
	iBindRet = bind(m_Socket, (SOCKADDR*)&myaddr, sizeof(struct sockaddr));
	if (iBindRet < 0)
	{
		printf("LINE [%d] , bind() called failed! GetLastError=%d\n", __LINE__ , WSAGetLastError());

		goto END;
	}
	*/

	addrServer.sin_addr.S_un.S_addr = inet_addr("192.168.3.16");
	addrServer.sin_family = AF_INET;
	addrServer.sin_port = htons(6000);

	ioctlsocket(m_Socket, FIONBIO, &ul);

	dwResult = connect(m_Socket, (SOCKADDR*)&addrServer, sizeof(SOCKADDR));
	if (0 != dwResult)
	{
		printf("connect failed, WSAGetLastError() = %d\n", WSAGetLastError());

	}
	else
	{
		printf("connect OK \n");
	}

	if ((SOCKET_ERROR == dwResult) && (WSAEWOULDBLOCK == WSAGetLastError()))
	{
		timeval stcTime;
		fd_set fd_WriteSet;
		fd_set fd_ExceptSet;
		stcTime.tv_sec = 2;   //等待超时
		stcTime.tv_usec = 0;
		int iErr = -1;
		int iLen = sizeof(iErr);

		FD_ZERO(&fd_WriteSet);
		FD_SET(m_Socket, &fd_WriteSet);

		FD_ZERO(&fd_ExceptSet);
		FD_SET(m_Socket, &fd_ExceptSet);

		dwResult = select(0, NULL, &fd_WriteSet, &fd_ExceptSet, &stcTime);
		if (dwResult == SOCKET_ERROR)
		{
			printf("LINE [%d] ,select socket() failed!GetLastError=%d\n", __LINE__, WSAGetLastError());
			dwRet = SOCKET_ERROR;
			goto END;
		}

		if (dwResult == 0)
		{
			printf("LINE [%d] ,connect timeout! GetLastError=%d\n", __LINE__, WSAGetLastError());
			goto END;
		}

		if (FD_ISSET(m_Socket, &fd_ExceptSet))
		{
			dwResult = getsockopt(m_Socket, SOL_SOCKET, SO_ERROR, (char *)&iErr, &iLen);

			if (SOCKET_ERROR == dwResult)
			{
				printf("LINE [%d] ,getsockopt fail!GetLastError=%d\n", __LINE__, WSAGetLastError());
				goto END;
			}
		}

		if (!FD_ISSET(m_Socket, &fd_WriteSet))
		{
			printf("LINE [%d] ,sock not in WriteSet!GetLastError=%d\n", __LINE__, WSAGetLastError());
			dwRet = SOCKET_ERROR;
			goto END;
		}
		
		
	}
	else if (dwResult != NO_ERROR)
	{
		printf("LINE [%d] ,connect failed!GetLastError=%d\n", __LINE__, WSAGetLastError());
		dwRet = WSAGetLastError();
		goto END;
	}

	printf("SUCCESS!!!\n");
	system("pause");
END:

	if (m_Socket != INVALID_SOCKET)
	{
		closesocket(m_Socket);
		m_Socket = INVALID_SOCKET;
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
