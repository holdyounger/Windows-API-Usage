#include <stdio.h>
#include <vector>
#include <stdlib.h>
#include <iostream>
#include <WinSock2.h>
#pragma comment (lib, "ws2_32.lib") // 加载ws32_32.dll
using namespace std;

int addPort(vector<SOCKET>& vecSock, int nPort)
{
	if (nPort < 0 || nPort > 0xFF)
	{
		return -1;
	}

	// 初始化 DLL
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	// 创建套接字
	SOCKET servSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

	// 绑定套接字
	struct sockaddr_in sockAddr;
	memset(&sockAddr, 0, sizeof(sockAddr)); // 每个套接字都用0填充
	sockAddr.sin_family = PF_INET; // 使用IPv4地址
	sockAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	sockAddr.sin_port = htons(1234);	// 端口
	bind(servSock, (SOCKADDR*)&sockAddr, sizeof(SOCKADDR));

	// 进入监听状态
	listen(servSock, 20);

	// 接收客户端请求
	SOCKADDR clntAddr;
	int nSize = sizeof(SOCKADDR);
	SOCKET clntSock = accept(servSock, (SOCKADDR*)&clntAddr, &nSize);
	vecSock.push_back(clntSock);
	vecSock.push_back(servSock);

	return 0;
}

int main(int argc, char* argv[])
{
	std::cout << "Hello World!\n";

	vector<SOCKET> vecSock;
ADD:
	int nComd = 2;
	int nPort = -1;
	int nRet = -1;
	printf("SERVER \t 请输入端口号，如果是-1则退出本程序：\n");
	scanf_s("%d", &nPort);

	switch (nPort)
	{
	case -1:
		goto END;
		break;
	default:
		if (0 != addPort(vecSock, nPort))
		{
			printf("Add this Port(%d)\n", nPort, GetLastError());
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
			closesocket(vecSock[i]);
		}
	}

	// 终止使用 DLL
	WSACleanup();

	return 0;
	return 0;
}