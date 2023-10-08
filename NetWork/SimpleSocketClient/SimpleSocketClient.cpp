// SimpleSocketClient.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <stdio.h>
#include <vector>
#include <stdlib.h>
#include <iostream>
#include <WinSock2.h>
#pragma comment (lib, "ws2_32.lib") // 加载ws32_32.dll
using namespace std;

int addPort(vector<SOCKET>& vecSock, int nPort)
{
	if (nPort < 0 || nPort > 0xFFFF)
	{
		return -1;
	}

	// 初始化DLL
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	// 创建套接字
	SOCKET sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	vecSock.push_back(sock);

	// 向服务器发起请求
	struct sockaddr_in sockAddr;
	memset(&sockAddr, 0, sizeof(sockAddr)); // 每个字节都用0填充 
	sockAddr.sin_family = PF_INET;
	sockAddr.sin_addr.s_addr = inet_addr("192.168.3.171");// 192.168.3.171  127.0.0.1
	sockAddr.sin_port = htons(nPort);
	int nRet = connect(sock, (SOCKADDR*)&sockAddr, sizeof(SOCKADDR));

	/*
		// 接收服务器传回的数据
		char szBuffer[MAXBYTE] = {0};
		recv(sock,szBuffer,MAXBYTE,NULL);

		while (strcmpi(szBuffer,"exit()") != 0)
		{
			if(recv(sock,szBuffer,MAXBYTE,NULL) < 0)
			{
				printf("客户端断开链接~\n");
				return 0;
			}

			// 输出接收到的数据
			printf("Message form server：%s\n", szBuffer);
		}*/


	return nRet;
}

int main()
{
	std::cout << "Hello World!\n";

	vector<SOCKET> vecSock;
ADD:
	int nComd = 2;
	int nPort = -1;
	int nRet = -1;
	printf("CLIENT \t 请输入端口号，如果是-1则退出本程序：\n");
	scanf_s("%d", &nPort);

	switch (nPort)
	{
	case -1:
		goto END;
		break;
	default:
		nRet = addPort(vecSock, nPort);
		if (0 != nRet)
		{
			printf("Add this Port(%d) Failed, GetLastError(%d)\n", nPort, GetLastError());
		}
		goto ADD;
	}


END:
	for (int i = 0; i < vecSock.size(); i++)
	{
		if (NULL != vecSock[i])
		{
			// 关闭套接字
			closesocket(vecSock[i]);
		}
	}

	// 终止使用 DLL
	WSACleanup();

	return 0;
}
