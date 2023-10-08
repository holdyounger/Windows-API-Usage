// GetAbailablePort.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "define.h"

#include <Windows.h>
#include <WinSock.h>
#include <tcpmib.h>
#include <IPHlpApi.h>

#include <set>
#include <memory>
#include <algorithm>

#pragma comment(lib, "WS2_32.lib")
#pragma comment(lib, "IPHlpApi.lib")

#define PORT_DOWN 49152
#define PORT_UP 65535

static std::set<uint16_t> GetAllTcpConnectionsPort()
{
	std::set<uint16_t> ret;
	ULONG size = 0;
	GetTcpTable(NULL, &size, TRUE);
	std::unique_ptr<char[]> buffer(new char[size]);

	PMIB_TCPTABLE tcpTable = reinterpret_cast<PMIB_TCPTABLE>(buffer.get());
	if (GetTcpTable(tcpTable, &size, FALSE) == NO_ERROR)
		for (size_t i = 0; i < tcpTable->dwNumEntries; i++)
			ret.insert(ntohs((uint16_t)tcpTable->table[i].dwLocalPort));
	// std::sort(std::begin(ret), std::end(ret));
	return ret;
}

static std::set<uint16_t> GetAllUdpConnectionsPort()
{
	std::set<uint16_t> ret;
	ULONG size = 0;
	GetUdpTable(NULL, &size, TRUE);
	std::unique_ptr<char[]> buffer(new char[size]);

	PMIB_UDPTABLE udpTable = reinterpret_cast<PMIB_UDPTABLE>(buffer.get());
	if (GetUdpTable(udpTable, &size, FALSE) == NO_ERROR)
		for (size_t i = 0; i < udpTable->dwNumEntries; i++)
			ret.insert(ntohs((uint16_t)udpTable->table[i].dwLocalPort));
	// std::sort(std::begin(ret), std::end(ret));
	return ret;
}

uint16_t FindAvailableTcpPort(uint16_t begin = PORT_DOWN, uint16_t end = PORT_UP)
{
	auto vec = GetAllTcpConnectionsPort();
	for (uint16_t port = begin; port != end; ++port)
		if (!std::binary_search(std::begin(vec), std::end(vec), port))
			return port;
	return 0;
}

uint16_t FindAvailableUdpPort(uint16_t begin = PORT_DOWN, uint16_t end = PORT_UP)
{
	auto vec = GetAllUdpConnectionsPort();
	for (uint16_t port = begin; port != end; ++port)
		if (!std::binary_search(std::begin(vec), std::end(vec), port))
			return port;
	return 0;
}

uint16_t FindAvailablePort(uint16_t begin = PORT_DOWN, uint16_t end = PORT_UP)
{
	/*
	auto vecTcp = GetAllTcpConnectionsPort(),
		vecUdp = GetAllUdpConnectionsPort();
	for (uint16_t port = begin; port != end; ++port)
		if (!std::binary_search(std::begin(vecTcp), std::end(vecTcp), port) &&
			!std::binary_search(std::begin(vecUdp), std::end(vecUdp), port))
			return port;
	return 0;
	*/
	auto vecTcp = GetAllTcpConnectionsPort();
	for (uint16_t port = begin; port != end; ++port)
		if (!std::binary_search(std::begin(vecTcp), std::end(vecTcp), port))
			return port;
	return 0;
}

int GetAbailablePort()
{

	//std::cout << "Tcp Available port : " << FindAvailableTcpPort() << "\n";
	//std::cout << "Udp Available port : " << FindAvailableUdpPort() << "\n";
	std::cout << "Available port : " << FindAvailablePort() << "\n";

	return 0;
}