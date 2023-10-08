// GetExtendedTcpTabel.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
// Need to link with Iphlpapi.lib and Ws2_32.lib
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <stdio.h>
#include <algorithm>
#include <set>
#include <vector>

#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "ws2_32.lib")
using namespace std;

int main()
{
	PMIB_TCPTABLE_OWNER_PID pTcpTable(NULL);
	DWORD dwSize(0);
	GetExtendedTcpTable(pTcpTable, &dwSize, TRUE, AF_INET, TCP_TABLE_OWNER_PID_ALL, 0);
	pTcpTable = (MIB_TCPTABLE_OWNER_PID *)new char[dwSize];//重新分配缓冲区

	if (GetExtendedTcpTable(pTcpTable, &dwSize, TRUE, AF_INET, TCP_TABLE_OWNER_PID_ALL, 0) != NO_ERROR)
	{
		delete pTcpTable;
		return 0;
	}
	int nNum = (int)pTcpTable->dwNumEntries; //TCP连接的数目
	for (int i = 0; i < nNum; i++)
	{
		printf("本地地址：%s:%d  远程地址：%s:%d  状态：%d  进程ID:%d\n",
			inet_ntoa(*(in_addr*)& pTcpTable->table[i].dwLocalAddr), //本地IP 地址
			htons(pTcpTable->table[i].dwLocalPort), //本地端口
			inet_ntoa(*(in_addr*)& pTcpTable->table[i].dwRemoteAddr), //远程IP地址
			htons(pTcpTable->table[i].dwRemotePort), //远程端口
			pTcpTable->table[i].dwState, //状态
			pTcpTable->table[i].dwOwningPid); //所属进程PID
	}
	delete pTcpTable;

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
