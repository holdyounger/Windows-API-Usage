// Macro_Test.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <windows.h>
// #define CheckFlagStart(X) L"-->Start Check " #X "<--"  
// #define CheckFlagEnd(X) L"-->End Check " #X "<--"

#define DebugStr(flag, product) printf("%S",L"-->" #flag " Check" #product "");

#define CheckFlagStart(X) DebugStr(Start, ##X); \
            DWORD dwStart_##X##_Time = GetTickCount64();
#define CheckFlagEnd(X) DebugStr(End, ##X); \
			DWORD dwend_##X##_Time = GetTickCount64(); \
            printf("耗时 %dms", (dwend_##X##_Time)-(dwStart_##X##_Time));

int main()
{
    std::cout << "Hello World!\n";
    CheckFlagStart(MySQL);
    CheckFlagEnd(MySQL);

    getchar();
}
