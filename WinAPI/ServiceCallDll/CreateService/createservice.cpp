#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <strsafe.h>
#include <Shlwapi.h>
#include <winsvc.h>

#pragma comment(lib, "Shlwapi.lib")
#pragma comment(lib, "Advapi32.lib")

// ��ʾ�ṩ����������
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
        _tprintf(_T("�ļ�·��������"));
        return;
    }

    SC_HANDLE schSCManager;
    SC_HANDLE schService;

    schSCManager = OpenSCManager(
        NULL,                    // ����Ǳ�������NULL�������Զ��дԶ�˻�����
        NULL, SC_MANAGER_ALL_ACCESS);

    if (NULL == schSCManager)
    {
        printf("OpenSCManager failed (%d)\n", GetLastError());
        return;
    }

    schService = CreateService(
        schSCManager,              // SCM��� 
        szServiceName,             // �������� 
        szServiceName,             // ��ʾ���� 
        SERVICE_ALL_ACCESS,        // ����Ȩ��
        SERVICE_WIN32_OWN_PROCESS, // ��������
        SERVICE_AUTO_START,        // ������������
        SERVICE_ERROR_NORMAL,      // ��������������
        szBinPath,                 // ����exe����·��
        NULL,                      // ��������Ҫ���ص��� 
        NULL,                      // tag��ʶ��
        NULL,                      // �����ķ���
        NULL,                      // �����˻���NULL��ʾΪlocalSystem
        NULL);                     // �˻�����

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
    // �鿴�����Ƿ��Ѿ�����
    if (!QueryServiceStatusEx(
        schService,                     // ������ 
        SC_STATUS_PROCESS_INFO,         // Ҫ��ȡ����Ϣ���𣬲�ͬ�ļ��𷵻ص���Ϣ��ϸ�̶Ȳ�ͬ
        (LPBYTE)&ssStatus,              // �������״̬�ṹ��
        sizeof(SERVICE_STATUS_PROCESS), // �������״̬�ṹ���С
        &dwBytesNeeded))                // ����Ҫ�Ķ����ַ�ռ�
    {
        _tprintf(_T("QueryServiceStatusEx failed (%d)\n"), GetLastError());
        CloseServiceHandle(schService);
        CloseServiceHandle(schSCManager);
        return;
    }

    // ��������Ѿ�������ֱ�ӷ���
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
    // ����ԭʼ��CheckPoint
    dwStartTickCount = GetTickCount();
    dwOldCheckPoint = ssStatus.dwCheckPoint;

    // ����������ڹرգ���ȴ��رպ�������
    while (ssStatus.dwCurrentState == SERVICE_STOP_PENDING)
    {
        dwWaitTime = ssStatus.dwWaitHint / 10;
        if (dwWaitTime < 1000)
            dwWaitTime = 1000;
        else if (dwWaitTime > 10000)
            dwWaitTime = 10000;

        Sleep(dwWaitTime);

        // �������Ƿ�ر�
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

    // ������������
    if (!StartService(
        schService,  // ������
        0,           // �������� 
        NULL))       // ����
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

    // ������״ֱ̬������������Ϊֹ 
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

    // �������Ƿ�����
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
    // �������Ƿ��Ѿ��ڹر�״̬
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
    // ����������ڹرգ��ȴ�����ȫ�ر�
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

    // ��������������й����У���Ҫ�ȹر������÷���ķ���
    StopDependentServices(schSCManager, schService);

    // ����һ�������˳�֪ͨ���������
    if (!ControlService(
        schService,
        SERVICE_CONTROL_STOP,
        (LPSERVICE_STATUS)&ssp))
    {
        _tprintf(_T("ControlService failed (%d)\n"), GetLastError());
        goto stop_cleanup;
    }

    // �ȴ�����ر�
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

    // �򿪷����ȡ������
    schService = OpenService(schSCManager, szServiceName, DELETE);
    if (schService == NULL)
    {
        _tprintf(_T("OpenService failed (%d)\n"), GetLastError());
        CloseServiceHandle(schSCManager);
        return;
    }

    // ɾ������
    if (!DeleteService(schService))
    {
        _tprintf(_T("DeleteService failed (%d)\n"), GetLastError());
    }
    else
        _tprintf(_T("Service deleted successfully\n"));

    CloseServiceHandle(schService);
    CloseServiceHandle(schSCManager);
}