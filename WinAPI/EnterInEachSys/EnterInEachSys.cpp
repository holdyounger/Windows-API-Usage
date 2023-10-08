#include "stdafx.h"
#include <iostream>
#include <Strsafe.h>
#pragma comment(lib, "Version.lib")
using namespace std;
#define SERVICE_WIN32_OWN_PROCESS      0x00000010
#define SERVICE_WIN32_SHARE_PROCESS    0x00000020
#define SERVICE_WIN32                  (SERVICE_WIN32_OWN_PROCESS | \
	SERVICE_WIN32_SHARE_PROCESS)

int _tmain(int argc, _TCHAR* argv[])
{
	printf("1:|\n|2:|\r\n|");
	system("pause");

	cout <<"win32:" << SERVICE_WIN32 << endl;
	return 0;
}

