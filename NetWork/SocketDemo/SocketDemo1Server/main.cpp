#include <iostream>
#include <WinSock2.h>
#include <string>
#pragma  comment (lib,"ws2_32.lib") // 加载 ws32_32.dll
using namespace std;
int main()
{
	// 初始化 DLL
	WSADATA wsaData;
	WSAStartup( MAKEWORD(2,2), &wsaData);

	// 创建套接字
	SOCKET servSock = socket(PF_INET,SOCK_STREAM,IPPROTO_TCP);

	// 绑定套接字
	struct sockaddr_in sockAddr;
	memset (&sockAddr, 0, sizeof(sockAddr)); // 每个套接字都用0填充
	sockAddr.sin_family = PF_INET; // 使用IPv4地址
	sockAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	sockAddr.sin_port = htons(1234);	// 端口
	bind(servSock, (SOCKADDR*)&sockAddr, sizeof(SOCKADDR));

	// 进入监听状态
	listen(servSock,20);

	// 接收客户端请求
	SOCKADDR clntAddr;
	int nSize = sizeof(SOCKADDR);
	SOCKET clntSock = accept(servSock, (SOCKADDR*)&clntAddr, &nSize);

	// 向客户端发送数据
	char *str = new char[100];
	gets(str);
	send(clntSock, str, strlen(str)+sizeof(char),NULL);
	while (strcmpi(str,"exit()") != 0)
	{
		gets(str);
		send(clntSock, str, strlen(str)+sizeof(char),NULL);
	}

	// 关闭套接字
	closesocket(clntSock);
	closesocket(servSock);

	return 0;
}