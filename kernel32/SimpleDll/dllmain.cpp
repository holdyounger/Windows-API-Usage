// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "pch.h"

DWORD WINAPI ThreadProc()
{
   //  MessageBox(NULL, L"我已成功打入敌人内部 By Startu", L"报告首长", 0);
    OutputDebugStringW(L"注入成功");
    OutputDebugStringA("注入成功");
    return 0;
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadProc, NULL, 0, NULL);
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

