// GetTcpStatistics.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

//#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <stdio.h>

#pragma comment(lib, "iphlpapi.lib")
TCP_RESERVE_PORT_RANGE tcpDynamicPort;
#define MALLOC(x) HeapAlloc(GetProcessHeap(), 0, (x)) 
#define FREE(x) HeapFree(GetProcessHeap(), 0, (x))

/* Note: could also use malloc() and free() */

int GetTcpStatisticsTest()
{
	PMIB_TCPSTATS pTCPStats;
	DWORD dwRetVal = 0;

	pTCPStats = (MIB_TCPSTATS*)MALLOC(sizeof(MIB_TCPSTATS));
	if (pTCPStats == NULL) {
		printf("Error allocating memory\n");
		return 1;
	}

	if ((dwRetVal = GetTcpStatistics(pTCPStats)) == NO_ERROR) {
		printf("\tActive Opens: %ld\n", pTCPStats->dwActiveOpens);
		printf("\tPassive Opens: %ld\n", pTCPStats->dwPassiveOpens);
		printf("\tSegments Recv: %ld\n", pTCPStats->dwInSegs);
		printf("\tSegments Xmit: %ld\n", pTCPStats->dwOutSegs);
		printf("\tTotal # Conxs: %ld\n", pTCPStats->dwNumConns);
	}
	else {
		printf("GetTcpStatistics failed with error: %ld\n", dwRetVal);

		LPVOID lpMsgBuf;
		if (FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
			FORMAT_MESSAGE_FROM_SYSTEM |
			FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			dwRetVal,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
			(LPTSTR)&lpMsgBuf,
			0,
			NULL)) {
			printf("\tError: %s", lpMsgBuf);
		}
		LocalFree(lpMsgBuf);
	}

	if (pTCPStats)
		FREE(pTCPStats);
}