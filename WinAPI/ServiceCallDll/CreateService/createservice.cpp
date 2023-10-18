#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <strsafe.h>
#include <Shlwapi.h>
#include <winsvc.h>

#pragma comment(lib, "Shlwapi.lib")
#pragma comment(lib, "Advapi32.lib")

// 显示提供的所有命令
void DisplayUsage(int argc, TCHAR* argv[]);
void IntallService(int argc, TCHAR* argv[]);
void StartService(int argc, TCHAR* argv[]);
void StopService(int argc, TCHAR* argv[]);
BOOL StopDependentServices(SC_HANDLE schSCManager, SC_HANDLE schService);
void RemoveService(int argc, TCHAR* argv[]);


int _tmain(int argc, TCHAR* argv[])
{
    if (argc < 2)
    {
        DisplayUsage(argc, argv);
        return 0;
    }

    TCHAR szCommand[32] = { 0 };
    StringCchCopy(szCommand, 32, argv[1]);

    if (lstrcmpi(szCommand, _T("create")))
    {
        IntallService(argc, argv);
    }
    else if (lstrcmpi(szCommand, _T("start")))
    {
        StartService(argc, argv);
    }
    else if (lstrcmpi(szCommand, _T("stop")))
    {
        StopService(argc, argv);
    }
    else if (lstrcmpi(szCommand, _T("delete")))
    {
        RemoveService(argc, argv);
    }

    return 0;
}

void DisplayUsage(int argc, TCHAR* argv[])
{

}

void IntallService(int argc, TCHAR* argv[])
{
    if (argc != 4)
    {
        _tprintf(_T("sc create [service name] [binPath= ]"));
        return;
    }

    TCHAR szServiceName[256] = { 0 };
    TCHAR szBinPath[MAX_PATH] = { 0 };
    StringCchCopy(szServiceName, 256, argv[3]);
    StringCchCopy(szBinPath, MAX_PATH, argv[4]);

    if (!PathFileExists(szBinPath))
    {
        _tprintf(_T("文件路径不存在"));
        return;
    }

    SC_HANDLE schSCManager;
    SC_HANDLE schService;

    schSCManager = OpenSCManager(
        NULL,                    // 如果是本机传入NULL，如果是远程写远端机器名
        NULL, SC_MANAGER_ALL_ACCESS);

    if (NULL == schSCManager)
    {
        printf("OpenSCManager failed (%d)\n", GetLastError());
        return;
    }

    schService = CreateService(
        schSCManager,              // SCM句柄 
        szServiceName,             // 服务名称 
        szServiceName,             // 显示名称 
        SERVICE_ALL_ACCESS,        // 服务权限
        SERVICE_WIN32_OWN_PROCESS, // 服务类型
        SERVICE_AUTO_START,        // 服务启动类型
        SERVICE_ERROR_NORMAL,      // 服务错误控制类型
        szBinPath,                 // 服务exe所在路径
        NULL,                      // 服务所需要加载的组 
        NULL,                      // tag标识符
        NULL,                      // 依赖的服务
        NULL,                      // 服务账户，NULL表示为localSystem
        NULL);                     // 账户密码

    if (schService == NULL)
    {
        _tprintf(_T("CreateService failed (%d)\n"), GetLastError());
        CloseServiceHandle(schSCManager);
        return;
    }
    else
    {
        printf("Service installed successfully\n");
    }

    CloseServiceHandle(schService);
    CloseServiceHandle(schSCManager);
}

void StartService(int argc, TCHAR* argv[])
{
    if (argc != 3)
    {
        _tprintf(_T(" sc start [service name]"));
        return;
    }

    TCHAR szServiceName[256] = { 0 };
    StringCchCopy(szServiceName, 256, argv[3]);

    SC_HANDLE schSCManager;
    SC_HANDLE schService;
    schSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
    if (NULL == schSCManager)
    {
        _tprintf(_T("OpenSCManager failed (%d)\n"), GetLastError());
        return;
    }

    schService = OpenService(
        schSCManager,
        szServiceName,
        SERVICE_ALL_ACCESS);

    if (schService == NULL)
    {
        _tprintf(_T("OpenService failed (%d)\n"), GetLastError());
        CloseServiceHandle(schSCManager);
        return;
    }

    SERVICE_STATUS_PROCESS ssStatus;
    DWORD dwBytesNeeded;
    // 查看服务是否已经启动
    if (!QueryServiceStatusEx(
        schService,                     // 服务句柄 
        SC_STATUS_PROCESS_INFO,         // 要获取的信息级别，不同的级别返回的信息详细程度不同
        (LPBYTE)&ssStatus,              // 服务进程状态结构体
        sizeof(SERVICE_STATUS_PROCESS), // 服务进程状态结构体大小
        &dwBytesNeeded))                // 还需要的额外地址空间
    {
        _tprintf(_T("QueryServiceStatusEx failed (%d)\n"), GetLastError());
        CloseServiceHandle(schService);
        CloseServiceHandle(schSCManager);
        return;
    }

    // 如果服务已经启动，直接返回
    if (ssStatus.dwCurrentState != SERVICE_STOPPED &&
        ssStatus.dwCurrentState != SERVICE_STOP_PENDING)
    {
        _tprintf(_T("Cannot start the service because it is already running\n"));
        CloseServiceHandle(schService);
        CloseServiceHandle(schSCManager);
        return;
    }

    DWORD dwOldCheckPoint;
    DWORD dwStartTickCount;
    DWORD dwWaitTime;
    // 保存原始的CheckPoint
    dwStartTickCount = GetTickCount();
    dwOldCheckPoint = ssStatus.dwCheckPoint;

    // 如果服务正在关闭，则等待关闭后再启动
    while (ssStatus.dwCurrentState == SERVICE_STOP_PENDING)
    {
        dwWaitTime = ssStatus.dwWaitHint / 10;
        if (dwWaitTime < 1000)
            dwWaitTime = 1000;
        else if (dwWaitTime > 10000)
            dwWaitTime = 10000;

        Sleep(dwWaitTime);

        // 检查服务是否关闭
        if (!QueryServiceStatusEx(
            schService,
            SC_STATUS_PROCESS_INFO,
            (LPBYTE)&ssStatus,
            sizeof(SERVICE_STATUS_PROCESS),
            &dwBytesNeeded))
        {
            _tprintf(_T("QueryServiceStatusEx failed (%d)\n"), GetLastError());
            CloseServiceHandle(schService);
            CloseServiceHandle(schSCManager);
            return;
        }

        if (ssStatus.dwCheckPoint > dwOldCheckPoint)
        {
            dwStartTickCount = GetTickCount();
            dwOldCheckPoint = ssStatus.dwCheckPoint;
        }
        else
        {
            if (GetTickCount() - dwStartTickCount > ssStatus.dwWaitHint)
            {
                _tprintf(_T("Timeout waiting for service to stop\n"));
                CloseServiceHandle(schService);
                CloseServiceHandle(schSCManager);
                return;
            }
        }
    }

    // 尝试启动服务
    if (!StartService(
        schService,  // 服务句柄
        0,           // 参数个数 
        NULL))       // 参数
    {
        _tprintf(_T("StartService failed (%d)\n"), GetLastError());
        CloseServiceHandle(schService);
        CloseServiceHandle(schSCManager);
        return;
    }
    else
    {
        _tprintf(_T("Service start pending...\n"));
    }

    // 检查服务状态直到到服务启动为止 
    if (!QueryServiceStatusEx(
        schService,
        SC_STATUS_PROCESS_INFO,
        (LPBYTE)&ssStatus,
        sizeof(SERVICE_STATUS_PROCESS),
        &dwBytesNeeded))
    {
        _tprintf(_T("QueryServiceStatusEx failed (%d)\n"), GetLastError());
        CloseServiceHandle(schService);
        CloseServiceHandle(schSCManager);
        return;
    }

    dwStartTickCount = GetTickCount();
    dwOldCheckPoint = ssStatus.dwCheckPoint;
    while (ssStatus.dwCurrentState == SERVICE_START_PENDING)
    {
        dwWaitTime = ssStatus.dwWaitHint / 10;
        if (dwWaitTime < 1000)
            dwWaitTime = 1000;
        else if (dwWaitTime > 10000)
            dwWaitTime = 10000;

        Sleep(dwWaitTime);

        if (!QueryServiceStatusEx(
            schService,
            SC_STATUS_PROCESS_INFO,
            (LPBYTE)&ssStatus,
            sizeof(SERVICE_STATUS_PROCESS),
            &dwBytesNeeded))
        {
            _tprintf(_T("QueryServiceStatusEx failed (%d)\n"), GetLastError());
            break;
        }

        if (ssStatus.dwCheckPoint > dwOldCheckPoint)
        {
            dwStartTickCount = GetTickCount();
            dwOldCheckPoint = ssStatus.dwCheckPoint;
        }
        else
        {
            if (GetTickCount() - dwStartTickCount > ssStatus.dwWaitHint)
            {
                break;
            }
        }
    }

    // 检查服务是否启动
    if (ssStatus.dwCurrentState == SERVICE_RUNNING)
    {
        _tprintf(_T("Service started successfully.\n"));
    }
    else
    {
        _tprintf(_T("Service not started. \n"));
        _tprintf(_T("  Current State: %d\n"), ssStatus.dwCurrentState);
        _tprintf(_T("  Exit Code: %d\n"), ssStatus.dwWin32ExitCode);
        _tprintf(_T("  Check Point: %d\n"), ssStatus.dwCheckPoint);
        _tprintf(_T("  Wait Hint: %d\n"), ssStatus.dwWaitHint);
    }

    CloseServiceHandle(schService);
    CloseServiceHandle(schSCManager);
}

void StopService(int argc, TCHAR* argv[])
{
    if (argc != 3)
    {
        _tprintf(_T(" sc stop [service name]"));
        return;
    }

    TCHAR szServiceName[256] = { 0 };
    StringCchCopy(szServiceName, 256, argv[3]);

    DWORD dwStartTime;
    DWORD dwTimeout;
    DWORD dwWaitTime;

    SC_HANDLE schSCManager;
    SC_HANDLE schService;
    schSCManager = OpenSCManager(
        NULL,
        NULL,
        SC_MANAGER_ALL_ACCESS);

    if (NULL == schSCManager)
    {
        printf("OpenSCManager failed (%d)\n", GetLastError());
        return;
    }

    schService = OpenService(
        schSCManager,
        szServiceName,
        SERVICE_STOP |
        SERVICE_QUERY_STATUS |
        SERVICE_ENUMERATE_DEPENDENTS);

    if (schService == NULL)
    {
        _tprintf(_T("OpenService failed (%d)\n"), GetLastError());
        CloseServiceHandle(schSCManager);
        return;
    }

    SERVICE_STATUS_PROCESS ssp;
    DWORD dwBytesNeeded;
    // 检查服务是否已经在关闭状态
    if (!QueryServiceStatusEx(
        schService,
        SC_STATUS_PROCESS_INFO,
        (LPBYTE)&ssp,
        sizeof(SERVICE_STATUS_PROCESS),
        &dwBytesNeeded))
    {
        printf("QueryServiceStatusEx failed (%d)\n", GetLastError());
        goto stop_cleanup;
    }

    if (ssp.dwCurrentState == SERVICE_STOPPED)
    {
        _tprintf(_T("Service is already stopped.\n"));
        goto stop_cleanup;
    }

    dwStartTime = GetTickCount();
    dwTimeout = 30000;
    dwWaitTime;
    // 如果服务正在关闭，等待其完全关闭
    while (ssp.dwCurrentState == SERVICE_STOP_PENDING)
    {
        _tprintf(_T("Service stop pending...\n"));
        dwWaitTime = ssp.dwWaitHint / 10;

        if (dwWaitTime < 1000)
            dwWaitTime = 1000;
        else if (dwWaitTime > 10000)
            dwWaitTime = 10000;

        Sleep(dwWaitTime);

        if (!QueryServiceStatusEx(
            schService,
            SC_STATUS_PROCESS_INFO,
            (LPBYTE)&ssp,
            sizeof(SERVICE_STATUS_PROCESS),
            &dwBytesNeeded))
        {
            _tprintf(_T("QueryServiceStatusEx failed (%d)\n"), GetLastError());
            goto stop_cleanup;
        }

        if (ssp.dwCurrentState == SERVICE_STOPPED)
        {
            _tprintf(_T("Service stopped successfully.\n"));
            goto stop_cleanup;
        }

        if (GetTickCount() - dwStartTime > dwTimeout)
        {
            _tprintf(_T("Service stop timed out.\n"));
            goto stop_cleanup;
        }
    }

    // 如果服务正在运行过程中，需要先关闭依赖该服务的服务
    StopDependentServices(schSCManager, schService);

    // 发送一个服务退出通知给服务程序
    if (!ControlService(
        schService,
        SERVICE_CONTROL_STOP,
        (LPSERVICE_STATUS)&ssp))
    {
        _tprintf(_T("ControlService failed (%d)\n"), GetLastError());
        goto stop_cleanup;
    }

    // 等待服务关闭
    while (ssp.dwCurrentState != SERVICE_STOPPED)
    {
        Sleep(ssp.dwWaitHint);
        if (!QueryServiceStatusEx(
            schService,
            SC_STATUS_PROCESS_INFO,
            (LPBYTE)&ssp,
            sizeof(SERVICE_STATUS_PROCESS),
            &dwBytesNeeded))
        {
            _tprintf(_T("QueryServiceStatusEx failed (%d)\n"), GetLastError());
            goto stop_cleanup;
        }

        if (ssp.dwCurrentState == SERVICE_STOPPED)
            break;

        if (GetTickCount() - dwStartTime > dwTimeout)
        {
            _tprintf(_T("Wait timed out\n"));
            goto stop_cleanup;
        }
    }
    _tprintf(_T("Service stopped successfully\n"));

stop_cleanup:
    CloseServiceHandle(schService);
    CloseServiceHandle(schSCManager);
}

BOOL StopDependentServices(SC_HANDLE schSCManager, SC_HANDLE schService)
{
    DWORD i;
    DWORD dwBytesNeeded;
    DWORD dwCount;

    LPENUM_SERVICE_STATUS   lpDependencies = NULL;
    ENUM_SERVICE_STATUS     ess;
    SC_HANDLE               hDepService;
    SERVICE_STATUS_PROCESS  ssp;

    DWORD dwStartTime = GetTickCount();
    DWORD dwTimeout = 30000;

    if (EnumDependentServices(schService, SERVICE_ACTIVE,
        lpDependencies, 0, &dwBytesNeeded, &dwCount))
    {
        return TRUE;
    }
    else
    {
        if (GetLastError() != ERROR_MORE_DATA)
            return FALSE;

        // Allocate a buffer for the dependencies.
        lpDependencies = (LPENUM_SERVICE_STATUS)HeapAlloc(
            GetProcessHeap(), HEAP_ZERO_MEMORY, dwBytesNeeded);

        if (!lpDependencies)
            return FALSE;

        __try {
            // Enumerate the dependencies.
            if (!EnumDependentServices(schService, SERVICE_ACTIVE,
                lpDependencies, dwBytesNeeded, &dwBytesNeeded,
                &dwCount))
                return FALSE;

            for (i = 0; i < dwCount; i++)
            {
                ess = *(lpDependencies + i);
                // Open the service.
                hDepService = OpenService(schSCManager,
                    ess.lpServiceName,
                    SERVICE_STOP | SERVICE_QUERY_STATUS);

                if (!hDepService)
                    return FALSE;

                __try {
                    // Send a stop code.
                    if (!ControlService(hDepService,
                        SERVICE_CONTROL_STOP,
                        (LPSERVICE_STATUS)&ssp))
                        return FALSE;

                    // Wait for the service to stop.
                    while (ssp.dwCurrentState != SERVICE_STOPPED)
                    {
                        Sleep(ssp.dwWaitHint);
                        if (!QueryServiceStatusEx(
                            hDepService,
                            SC_STATUS_PROCESS_INFO,
                            (LPBYTE)&ssp,
                            sizeof(SERVICE_STATUS_PROCESS),
                            &dwBytesNeeded))
                            return FALSE;

                        if (ssp.dwCurrentState == SERVICE_STOPPED)
                            break;

                        if (GetTickCount() - dwStartTime > dwTimeout)
                            return FALSE;
                    }
                }
                __finally
                {
                    CloseServiceHandle(hDepService);
                }
            }
        }
        __finally
        {
            HeapFree(GetProcessHeap(), 0, lpDependencies);
        }
    }
    return TRUE;
}


void RemoveService(int argc, TCHAR* argv[])
{
    if (argc != 3)
    {
        _tprintf(_T("sc delete [service name]"));
        return;
    }

    TCHAR szServiceName[256] = { 0 };
    StringCchCopy(szServiceName, 256, argv[3]);

    SC_HANDLE schSCManager;
    SC_HANDLE schService;
    schSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);

    if (NULL == schSCManager)
    {
        _tprintf(_T("OpenSCManager failed (%d)\n"), GetLastError());
        return;
    }

    // 打开服务获取服务句柄
    schService = OpenService(schSCManager, szServiceName, DELETE);
    if (schService == NULL)
    {
        _tprintf(_T("OpenService failed (%d)\n"), GetLastError());
        CloseServiceHandle(schSCManager);
        return;
    }

    // 删除服务
    if (!DeleteService(schService))
    {
        _tprintf(_T("DeleteService failed (%d)\n"), GetLastError());
    }
    else
        _tprintf(_T("Service deleted successfully\n"));

    CloseServiceHandle(schService);
    CloseServiceHandle(schSCManager);
}