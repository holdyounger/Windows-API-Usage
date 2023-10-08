// FD_SET.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <WinSock2.h>
#include <stdio.h>
#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "ws2_32.lib")
using namespace std;
int main()
{
/*
	SOCKET m_Socket = {0};		// 定义一个socket对象
	fd_set fdset = {0};	// 声明并定义，如果不赋初值，fd_set中存储的则是随机值

	FD_ZERO(&fdset);
	
	FD_SET(4, &fdset);
	FD_SET(m_Socket, &fdset);
	FD_SET(2, &fdset);
	FD_SET(3, &fdset);
	FD_SET(7, &fdset);
	

	int isset = FD_ISSET(m_Socket, &fdset);
	printf("isset = %d\n", isset); // isset = 1

	FD_CLR(m_Socket, &fdset);

	isset = FD_ISSET(m_Socket, &fdset); // isset = 0
	printf("isset = %d\n", isset);*/


	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);

	SOCKADDR_IN addrServer;
	SOCKET Socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	fd_set readset, writeset;

	addrServer.sin_addr.S_un.S_addr = inet_addr("192.168.3.16");
	addrServer.sin_family = AF_INET;
	addrServer.sin_port = htons(6000);
	DWORD dwResult = connect(Socket, (SOCKADDR*)&addrServer, sizeof(SOCKADDR));
	FD_ZERO(&readset);
	FD_ZERO(&writeset);

	FD_SET(Socket, &readset);
	FD_SET(Socket, &writeset);

	/*
	int isset = FD_ISSET(Socket, &readset); // isset = 0
	printf("isset = %d\n", isset);
	isset = FD_ISSET(Socket, &writeset); // isset = 0
	printf("isset = %d\n", isset);
	*/

	int nRet = select(0, &readset, &writeset, NULL, NULL);
	cout << "The Ret of Select is " << nRet << endl;
	if (FD_ISSET(Socket, &readset))
	{
		cout << "sock in readset!" << endl;
	}
	if (FD_ISSET(Socket, &writeset))
	{
		cout << "sock in writeset!" << endl;
	}

	system("pause");
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
