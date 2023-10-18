#include <windows.h>
#include <tchar.h>
#include <strsafe.h>
#include <atlstr.h>

#define MyServiceName L"MyService"

bool                    g_bPausedFlag = false;
HANDLE                  g_hSvcStopEvent = NULL;
SERVICE_STATUS          g_MyServiceStatus;
SERVICE_STATUS_HANDLE   g_hMyServiceStatusHandle;

void __stdcall MyServiceMain(DWORD  dwArgc, LPTSTR* lpszArgv);
DWORD __stdcall MyServiceHandlerEx(DWORD  dwControl, DWORD  dwEventType, LPVOID lpEventData, LPVOID lpContext);
void ReportServiceStatus(DWORD dwCurrentState, DWORD dwWin32ExitCode, DWORD dwWaitHint);
DWORD MyServiceInitialization(DWORD  dwArgc, LPTSTR* lpszArgv);
DWORD MyServiceWorker(DWORD dwArgc, LPTSTR* lpszArgv);

int _tmain(int argc, TCHAR* argv[])
{
    SERVICE_TABLE_ENTRY dispatchTable[] = {
        {(LPWSTR)MyServiceName, (LPSERVICE_MAIN_FUNCTION)MyServiceMain},
        {NULL, NULL} };

    if (!StartServiceCtrlDispatcher(dispatchTable))
    {
        // cleaning worker
    }

    return 0;
}

void __stdcall MyServiceMain(DWORD  dwArgc, LPTSTR* lpszArgv)
{
    g_hSvcStopEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    g_hMyServiceStatusHandle = RegisterServiceCtrlHandlerEx(MyServiceName, (LPHANDLER_FUNCTION_EX)MyServiceHandlerEx, NULL);
    if (g_hMyServiceStatusHandle == NULL)
        return;

    g_MyServiceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
    g_MyServiceStatus.dwServiceSpecificExitCode = 0;
    ReportServiceStatus(SERVICE_START_PENDING, 0, 0);

    DWORD dwRet = MyServiceInitialization(dwArgc, lpszArgv);
    if (dwRet != NO_ERROR)
        ReportServiceStatus(SERVICE_STOPPED, dwRet, 0);
    ReportServiceStatus(SERVICE_RUNNING, 0, 0);

    MyServiceWorker(dwArgc, lpszArgv);

    while (true)
    {
        WaitForSingleObject(g_hSvcStopEvent, INFINITE);
        ReportServiceStatus(SERVICE_STOPPED, 0, 0);
        return;
    }
}

DWORD __stdcall MyServiceHandlerEx(DWORD  dwControl, DWORD  dwEventType, LPVOID lpEventData, LPVOID lpContext)
{
    switch (dwControl)
    {
    case SERVICE_CONTROL_SHUTDOWN:
    case SERVICE_CONTROL_STOP:
        ReportServiceStatus(SERVICE_STOP_PENDING, 0, 0);
        SetEvent(g_hSvcStopEvent);
        break;
    case SERVICE_CONTROL_PAUSE:
        ReportServiceStatus(SERVICE_PAUSE_PENDING, 0, 0);
        g_bPausedFlag = true;
        break;
    case SERVICE_CONTROL_CONTINUE:
        ReportServiceStatus(SERVICE_CONTINUE_PENDING, 0, 0);
        g_bPausedFlag = false;
        break;
    case SERVICE_CONTROL_INTERROGATE:
        break;
    default:
        break;
    }
    return 0;
}

// 执行一些初始化过程
DWORD MyServiceInitialization(DWORD  dwArgc, LPTSTR* lpszArgv)
{
    OutputDebugStringA("------------------Start----------------");

    return NO_ERROR;
}

// 服务工作执行函数
DWORD MyServiceWorker(DWORD dwArgc, LPTSTR* lpszArgv)
{
    HINSTANCE hDLL; // Handle to DLL
    using Face = void (*)();

    hDLL = LoadLibrary(L"CallMysqlDll.dll");
    OutputDebugStringA("------------------LoadLibrary----------------");
    if (hDLL != NULL)
    {
        OutputDebugStringA("------------------LoadLibrary Success----------------");

        Face faceSum = (Face)GetProcAddress(hDLL,
            "ConnMySQL");

        if (!faceSum)
        {
            // handle the error
            OutputDebugStringA("------------------LoadLibrary Call Function Failed----------------");
            FreeLibrary(hDLL);
            return 0;
        }
        else
        {
            OutputDebugStringA("------------------LoadLibrary Call Function----------------");
            // call the function
            // faceSum();
        }
    }
    else
    {
        OutputDebugStringA("------------------LoadLibrary failed----------------");
        DWORD error = GetLastError();
        CString strError = "";
        strError.Format(L"----------------GetLastError=%d", error);
        OutputDebugString(strError.GetBuffer());

    }

    return 0;
}

// 向服务控制管理器报告状态信息
void ReportServiceStatus(DWORD dwCurrentState, DWORD dwWin32ExitCode, DWORD dwWaitHint)
{
    static DWORD dwCheckPoint = 1;

    g_MyServiceStatus.dwCurrentState = dwCurrentState;
    g_MyServiceStatus.dwWin32ExitCode = dwWin32ExitCode;
    g_MyServiceStatus.dwWaitHint = dwWaitHint;

    if (dwCurrentState == SERVICE_START_PENDING)
        g_MyServiceStatus.dwControlsAccepted = 0;
    else
        g_MyServiceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_PAUSE_CONTINUE;

    if ((dwCurrentState == SERVICE_RUNNING) ||
        (dwCurrentState == SERVICE_STOPPED))
        g_MyServiceStatus.dwCheckPoint = 0;
    else
        g_MyServiceStatus.dwCheckPoint = dwCheckPoint++;

    SetServiceStatus(g_hMyServiceStatusHandle, &g_MyServiceStatus);
}