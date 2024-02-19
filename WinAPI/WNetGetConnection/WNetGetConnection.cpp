// WNetGetConnection.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <windows.h>
#include <atlstr.h>
#pragma comment(lib, "mpr.lib")

#include <stdio.h>
#include <winnetwk.h>

int main()
{
    TCHAR szDeviceName[80];
    DWORD dwResult, cchBuff = sizeof(szDeviceName);

    // Call the WNetGetConnection function.
    //
    dwResult = WNetGetConnection(_T("z:"),
        szDeviceName,
        &cchBuff);

    switch (dwResult)
    {
        //
        // Print the connection name or process errors.
        //
    case NO_ERROR:
        printf("Connection name: %S\n", szDeviceName);
        std::cout << szDeviceName << std::endl;
        break;
        //
        // The device is not a redirected device.
        //
    case ERROR_NOT_CONNECTED:
        printf("Device z: not connected.\n");
        break;
        //
        // The device is not currently connected, but it is a persistent connection.
        //
    case ERROR_CONNECTION_UNAVAIL:
        printf("Connection unavailable.\n");
        break;
        //
        // Handle the error.
        //
    default:
        printf("WNetGetConnection failed.\n");
    }

    system("pause");
}
