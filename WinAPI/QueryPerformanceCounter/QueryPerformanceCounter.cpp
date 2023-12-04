#include <stdio.h>
#include <Windows.h>

LONGLONG GetLastTime()
{
    // CPU频率
    LARGE_INTEGER liQPF;

    // 记录开始和结束时间
    LARGE_INTEGER liStartTime, liEndTime;

    // 记录过程时间
    LONGLONG llLastTime;

    // 获取CPU频率
    QueryPerformanceFrequency(&liQPF);

    // 获取开始时间
    QueryPerformanceCounter(&liStartTime);

    Sleep(10);

    // 获取结束时间
    QueryPerformanceCounter(&liEndTime);

    // 计算持续时间(us)
    llLastTime = 1000000 * (liEndTime.QuadPart - liStartTime.QuadPart) / liQPF.QuadPart;

    return llLastTime;
}

int main(void)
{
    DWORD start, stop;
    start = GetTickCount();
    Sleep(10);
    stop = GetTickCount();
    printf("%d ms\n", stop - start); //GetTickCount()计算得到的时间间隔精度为15ms，例如计算10ms间隔结果为0
    printf("%d us\n", GetLastTime());//应用程序中时间间隔计算精确到us，QueryPerformanceCounter()
    return 0;
}