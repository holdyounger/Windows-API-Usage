#pragma once
#include <iostream>
#include<iomanip>
using namespace std;

#define NAMEPRINTFORMAT(X) std::left << setw(60)<< (#X) << "\t\t" << X

int GetAbailablePort();

int GetIpForwardTableTest();

int GetIsDeleteDftRoute();

int GetNetInfo();

int GetPort();

int GetTcpStatisticsTest();

int IpConvert();