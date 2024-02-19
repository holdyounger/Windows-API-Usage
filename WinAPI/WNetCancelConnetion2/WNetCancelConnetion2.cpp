// WNetCancelConnetion2.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <windows.h>
#include <atlstr.h>
#pragma comment(lib, "mpr.lib")

#include <stdio.h>
#include <winnetwk.h>
int main()
{
    DWORD dwResult;

    // Call the WNetCancelConnection2 function, specifying
    //  that the connection should no longer be a persistent one.
    //
    dwResult = WNetCancelConnection2(_T("z:"),
        CONNECT_UPDATE_PROFILE, // remove connection from profile 
        FALSE);                 // fail if open files or jobs 

    // Process errors.
    //  The device is not a local redirected device.
    //
    if (dwResult == ERROR_NOT_CONNECTED)
    {
        printf("Drive z: not connected.\n");
        return dwResult;
    }

    // Call an application-defined error handler.
    //
    else if (dwResult != NO_ERROR)
    {
        printf("WNetCancelConnection2 failed.\n");
        return dwResult;
    }
    //
    // Otherwise, report canceling the connection.
    //
    printf("Connection closed for z: drive.\n");

    system("pause");
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
