// ComHelper.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include "ComHelper.h"

int main()
{
    std::cout << "Hello World!\n";

    WP_PROCESS_INFO_EX wppi = { 0 };
    CComHelper::GetProcessInfoByNameEx(L"mysqld.exe", wppi);

    system("pause");

    return 0;
}