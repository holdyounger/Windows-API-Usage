#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <iostream>
#include<iomanip>
#include<string>
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>

#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "ws2_32.lib")

using namespace std;

HANDLE hEvent = NULL;

#define NAMEPRINTFORMAT(X) std::left << setw(60)<< #X << "\t\t" << X

std::string ipToString(DWORD dwIP)
{
	std::string strDestIp = "";
	std::string strMaskIp = "";

	struct in_addr network;
	network.S_un.S_addr = dwIP;    //为s_addr赋值--网络字节序
	strDestIp = inet_ntoa(network);

	return strDestIp;
}


void CALLBACK RouteChanged1(void* CallerContext, PMIB_IPFORWARD_ROW2 Table, MIB_NOTIFICATION_TYPE NotificationType)
{
	using context = void(*)(DWORD);

	context ctx = (context)CallerContext;

	auto PrintTable = [](MIB_IPFORWARD_ROW2 table) {
		cout << "---------------" << endl;
		cout << NAMEPRINTFORMAT(table.InterfaceLuid.Info.IfType) << endl;

		cout << NAMEPRINTFORMAT(table.InterfaceLuid.Info.NetLuidIndex) << endl;
		cout << NAMEPRINTFORMAT(table.InterfaceLuid.Info.Reserved) << endl;
		cout << NAMEPRINTFORMAT(table.InterfaceLuid.Value) << endl;

		cout << NAMEPRINTFORMAT(table.InterfaceIndex) << endl;


		cout << NAMEPRINTFORMAT(table.DestinationPrefix.Prefix.si_family) << endl;
		cout << NAMEPRINTFORMAT((unsigned)table.DestinationPrefix.PrefixLength) << endl;
		cout << NAMEPRINTFORMAT(table.DestinationPrefix.Prefix.Ipv4.sin_addr.S_un.S_addr) << "\t" << ipToString(table.DestinationPrefix.Prefix.Ipv4.sin_addr.S_un.S_addr) << endl;
		cout << NAMEPRINTFORMAT(table.DestinationPrefix.Prefix.Ipv4.sin_family) << endl;
		cout << NAMEPRINTFORMAT(table.DestinationPrefix.Prefix.Ipv4.sin_port) << endl;
		cout << NAMEPRINTFORMAT(table.DestinationPrefix.Prefix.Ipv4.sin_zero) << endl;

		cout << NAMEPRINTFORMAT(table.DestinationPrefix.Prefix.Ipv6.sin6_addr.u.Byte) << endl;
		cout << NAMEPRINTFORMAT(table.DestinationPrefix.Prefix.Ipv6.sin6_addr.u.Word) << endl;
		cout << NAMEPRINTFORMAT(table.DestinationPrefix.Prefix.Ipv6.sin6_family) << endl;
		cout << NAMEPRINTFORMAT(table.DestinationPrefix.Prefix.Ipv6.sin6_flowinfo) << endl;
		cout << NAMEPRINTFORMAT(table.DestinationPrefix.Prefix.Ipv6.sin6_port) << endl;
		cout << NAMEPRINTFORMAT(table.DestinationPrefix.Prefix.Ipv6.sin6_scope_id) << endl;
		cout << NAMEPRINTFORMAT(table.DestinationPrefix.Prefix.Ipv6.sin6_scope_struct.Level) << endl;
		cout << NAMEPRINTFORMAT(table.DestinationPrefix.Prefix.Ipv6.sin6_scope_struct.Value) << endl;
		cout << NAMEPRINTFORMAT(table.DestinationPrefix.Prefix.Ipv6.sin6_scope_struct.Zone) << endl;
		

		cout << NAMEPRINTFORMAT(table.NextHop.Ipv4.sin_addr.S_un.S_addr) << "\t" << ipToString(table.NextHop.Ipv4.sin_addr.S_un.S_addr) << endl;
		cout << NAMEPRINTFORMAT(table.NextHop.Ipv6.sin6_addr.u.Byte) << endl;
		cout << NAMEPRINTFORMAT(table.NextHop.Ipv6.sin6_addr.u.Word) << endl;
		cout << NAMEPRINTFORMAT(table.SitePrefixLength) << endl;
		cout << NAMEPRINTFORMAT(table.ValidLifetime) << endl;
		cout << NAMEPRINTFORMAT(table.PreferredLifetime) << endl;
		cout << NAMEPRINTFORMAT(table.Metric) << endl;
		cout << NAMEPRINTFORMAT(table.Protocol) << endl;
		cout << NAMEPRINTFORMAT(table.Loopback) << endl;
		cout << NAMEPRINTFORMAT(table.AutoconfigureAddress) << endl;
		cout << NAMEPRINTFORMAT(table.Immortal) << endl;
		cout << NAMEPRINTFORMAT(table.Age) << endl;
		cout << NAMEPRINTFORMAT(table.Origin) << endl;
		// cout << NAMEPRINTFORMAT(table) << endl;

		cout << "---------------" << endl << endl;
	};
	// 处理路由表变更通知
	switch (NotificationType)
	{
	case MibParameterNotification:
		cout << "1-- 参数被更改" << endl;
		break;
	case MibAddInstance:
		cout << "2-- 添加" << endl;
		PrintTable(*Table);

		ctx(Table->DestinationPrefix.Prefix.Ipv4.sin_addr.S_un.S_addr);

		SetEvent(hEvent);
		break;
	case MibDeleteInstance:
		cout << "3-- 删除" << endl;
		PrintTable(*Table);
		
		// 回调
		ctx(Table->DestinationPrefix.Prefix.Ipv4.sin_addr.S_un.S_addr);

		SetEvent(hEvent);
		break;
	case MibInitialNotification:
		cout << "4-- 函数已注册" << endl;
		break;

	default:
		break;
	}
}


void checkIsMyRoute (DWORD ip)
{
	cout << NAMEPRINTFORMAT(ipToString(ip)) << endl;
};

void main()
{
	HANDLE hNotify = NULL;
	HANDLE hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (hEvent == nullptr)
	{
		cout << endl;
	}
	PVOID callerContext = static_cast<PVOID>(&checkIsMyRoute);
	BOOLEAN bSuccess = TRUE;
	ULONG pdwPrevNotified = 0;

	if (callerContext != nullptr && NotifyRouteChange2(AF_UNSPEC, &RouteChanged1, callerContext, bSuccess, &hNotify) != NO_ERROR) {
		printf("Could not register for route change notifications\n");
		return ;
	}

#if 1
	while (bSuccess && hEvent) {
		DWORD dwWait = WaitForSingleObject(hEvent, INFINITE);
		if (dwWait == WAIT_OBJECT_0) {
			// 接收到异步通知，处理结果
			break;
		}
	}
#endif

	// 取消监控
	CancelMibChangeNotify2(hNotify);

	int i = 0;
	cout << i;

	return ;
}