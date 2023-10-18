// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "pch.h"
#include <mysqlx/xdevapi.h>
#include <sstream>
#include <atlstr.h>
#ifdef __cplusplus
extern "C"
{
#endif	 

#define MYIMAPI extern "C" __declspec(dllimport)

#define MYEXAPI extern "C" __declspec(dllexport)

#ifdef __cplusplus
}
#endif
using namespace mysqlx;
#ifdef _DEBUG
#pragma comment(lib, "mysqlcppconn8-static-debug-openssl3-mtd.lib")    
#pragma comment(lib, "dnsapi.lib")
#else
#pragma comment(lib, "mysqlcppconn8-static-Release-openssl3-mt.lib")
#pragma comment(lib, "dnsapi.lib")
#endif // DEBUG

DWORD WINAPI ThreadProc()
{
    MessageBox(NULL, L"我已成功打入敌人内部 By Startu", L"报告首长", 0);
    return 0;
}

MYEXAPI void  ConnMySQL()
{
    CStringA username = "root";
    CStringA password = "Admin@2022";
    try {
        // 改成你的信息
        Session sess(SessionOption::USER, username.GetBuffer(),
            SessionOption::PWD, password.GetBuffer(),
            SessionOption::HOST, "localhost",
            SessionOption::PORT, 33060,
            SessionOption::DB, "mysql");

        auto result = sess.sql("select * from user").execute();

        // MessageBox(NULL, L"我已成功打入敌人内部 By Startu", L"报告首长", 0);
        OutputDebugStringA("[CallMysqlDll]----------------------------------");
        for (auto row : result.fetchAll()) {
            std::ostringstream oss;
            oss << row[0] << " " << row[1] << " " << row[2] << "\n";
            OutputDebugStringA(oss.str().c_str());
        }

    }
    catch (const std::exception& e) {
        OutputDebugStringA(e.what());
    }
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

