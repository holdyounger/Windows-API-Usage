#include "define.h"
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#include <ws2tcpip.h>
#include <ws2def.h>
#include <iphlpapi.h>
#include <stdio.h>
#include <bitset> //输出二进制的头文件

#pragma comment(lib, "iphlpapi.lib")

int maskToDigit(std::string strIp)
{
	int nMaskNum = 0;
	DWORD dwMask = 0;
	uint32_t ip = inet_addr(strIp.c_str());

	if (ip == INADDR_NONE)
	{
		return -1;
	}

	dwMask = (DWORD)ip;

	while (dwMask)
	{
		if (ip & 0x80000000)
		{
			nMaskNum++;
		}
		else
		{
			return nMaskNum;
		}

		dwMask <<= 1;
	}

	return nMaskNum;
}

int maskToDigit(DWORD ip)
{
	int nMaskNum = 0;
	cout << "二进制： " << bitset<sizeof(ip) * 8>(ip) << endl;
	bitset<32> bitBinary = bitset<sizeof(ip) * 8>(ip);
	cout << bitBinary.count();
	while (ip)
	{
		if (ip & 1)
		{	
			nMaskNum++;
		}

		ip >>= 1;
	}

	return nMaskNum;
}

string ipToString(DWORD dwIP)
{
	std::string strDestIp = "";
	std::string strMaskIp = "";

	struct in_addr network;
	network.S_un.S_addr = dwIP;    //为s_addr赋值--网络字节序
	strDestIp = inet_ntoa(network);

	return strDestIp;
}

uint32_t stringToIP(std::string strIP)
{
	int nMaskNum = 0;
	DWORD ip = inet_addr(strIP.c_str());

	if (ip == INADDR_NONE)
	{
		return -1;
	}

	return ip;
}

int IpConvert() 
{
	uint32_t ip = stringToIP("10.41.0.0");
	uint32_t ip1 = stringToIP("255.255.255.0");
	uint32_t ip2 = stringToIP("255.255.255.128"); // 192
	uint32_t ip3 = stringToIP("255.255.255.255"); // 4294967295

	string strip = ipToString(ip);
	string strip1 = ipToString(ip1);
	string strip2 = ipToString(ip2);
	string strip3 = ipToString(ip3);

	cout << NAMEPRINTFORMAT((ip)) << endl;
	cout << NAMEPRINTFORMAT((ip1)) << endl;
	cout << NAMEPRINTFORMAT((ip2)) << endl;
	cout << NAMEPRINTFORMAT((ip3)) << endl;
	cout << endl;
	cout << NAMEPRINTFORMAT(maskToDigit(ip)) << endl;
	cout << NAMEPRINTFORMAT(maskToDigit(ip1)) << endl;
	cout << NAMEPRINTFORMAT(maskToDigit(ip2)) << endl;
	cout << NAMEPRINTFORMAT(maskToDigit(ip3)) << endl;

	cout << endl;

	cout << NAMEPRINTFORMAT(maskToDigit(strip)) << endl;
	cout << NAMEPRINTFORMAT(maskToDigit(strip1)) << endl;
	cout << NAMEPRINTFORMAT(maskToDigit(strip2)) << endl;
	cout << NAMEPRINTFORMAT(maskToDigit(strip3)) << endl;

	return 0;
}