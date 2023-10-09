// ExpandEnvironmentStrings.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//
#include <windows.h>
#include <iostream>


int main(int argc, char *argv[])
{
    LPSTR dstlstr;

    dstlstr = (LPSTR)malloc(sizeof(LPSTR));

    for (int i = 1; i < argc; i++)
    {
        ExpandEnvironmentStringsA(argv[i], dstlstr, sizeof(argv[i]));
        std::cout << "argv[i]: " << argv[i] << "\t" << "argv[i][expand]:" << dstlstr << std::endl;
        memset(dstlstr, 0, sizeof(LPSTR));
    }
    
}
