//RegMonitorClass.cpp
#include "CRegMonitorClass.h"
#include <assert.h>

#pragma comment(lib, "Advapi32.lib")
#define TRACE printf


CRegMonitorClass::CRegMonitorClass()
{
    m_hMonRegKey = NULL;
    m_hExitEvent = NULL;
    m_hMonEvent = NULL;
    m_hMonThread = NULL;
}

CRegMonitorClass::~CRegMonitorClass()
{
    StopMonitor();
}

//停止监控
BOOL CRegMonitorClass::StopMonitor()
{
    if (m_hExitEvent)
    {
        SetEvent(m_hExitEvent);
    }
    if (m_hMonThread)
    {
        WaitForSingleObject(m_hMonThread, 2000);
        CloseHandle(m_hMonThread);
        m_hMonThread = NULL;
    }
    if (m_hExitEvent)
    {
        CloseHandle(m_hExitEvent);
        m_hExitEvent = NULL;
    }
    if (m_hMonEvent)
    {
        CloseHandle(m_hMonEvent);
        m_hMonEvent = NULL;
    }
    if (m_hMonRegKey)
    {
        RegCloseKey(m_hMonRegKey);
        m_hMonRegKey = NULL;
    }

    _DbgPrintf(("StopMonitor\n"));

    return TRUE;
}

//启动监控
//成功是返回TURE
BOOL CRegMonitorClass::StartMonitor(HKEY hPrmKey, LPCTSTR szKey)
{
    BOOL bRet = FALSE;
    StopMonitor();

    do
    {
        //打开注册表
        DWORD dwRet = RegOpenKeyEx(hPrmKey, szKey, 0, KEY_ALL_ACCESS, &m_hMonRegKey);
        if (dwRet != ERROR_SUCCESS || m_hMonRegKey == NULL)
        {
            _DbgPrintf(("RegOpenKeyEx failed %u\n"), dwRet);
            bRet = -1;
            break;
        }

        //创建退出事件
        if (m_hExitEvent == NULL)
        {
            m_hExitEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
        }
        if (m_hExitEvent == NULL)
        {
            _DbgPrintf(("Create exit Event failed %u\n"), GetLastError());
            bRet = -2;
            break;
        }

        //创建监控变化事件
        if (m_hMonEvent == NULL)
        {
            m_hMonEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
        }
        if (m_hMonEvent == NULL)
        {
            _DbgPrintf(("Create monitor Event failed %u\n"), GetLastError());
            bRet = -3;
            break;
        }

        //启动监控线程
        if (m_hMonThread == NULL)
        {
            m_hMonThread = CreateThread(NULL, 0, RegMonThread, this, 0, NULL);
        }
        if (m_hMonThread == NULL)
        {
            _DbgPrintf(("CreateThread failed %u\n"), GetLastError());
            bRet = -4;
            break;
        }

        _DbgPrintf(("Start register monitor succeed\n"));

        bRet = TRUE;

    } while (0);

    //失败时关闭监控
    if (bRet <= 0)
    {
        StopMonitor();
    }

    return bRet;
}

//监控注册表功能函数
DWORD CRegMonitorClass::RegMonFunction()
{
    assert(m_hMonEvent);
    assert(m_hMonRegKey);
    assert(m_hExitEvent);

    BOOL bEixtFlag = FALSE;
    while ((!bEixtFlag))
    {
        //重置事件
        ResetEvent(m_hMonEvent);

        //注册事件
        DWORD dwRet = RegNotifyChangeKeyValue(m_hMonRegKey,
            TRUE, REG_LEGAL_CHANGE_FILTER, m_hMonEvent, TRUE);

        if (dwRet != ERROR_SUCCESS)
        {
            TRACE(("RegNotifyChangeKeyValue failed %u\n"), dwRet);
            break;
        }

        //等待事件
        HANDLE hEventArray[] = { m_hExitEvent, m_hMonEvent };
        DWORD dwWait = WaitForMultipleObjects(2, hEventArray, FALSE, INFINITE);
        switch (dwWait)
        {
            case(WAIT_OBJECT_0 + 0):  //exit event
            {
                bEixtFlag = 1;
                break;
            }
            case(WAIT_OBJECT_0 + 1): //done event
            {
                TRACE(("RegNotifyChangeKeyValue Notifyed!\n"));
                //这里添加处理

                break;
            }
            default: //unexpected
            {
                bEixtFlag = -1;
                TRACE(("WaitForMultipleObjects failed %u\n"), dwWait);
                break;
            }
        }
    }

    return 0;
}

//监控线程
DWORD CALLBACK CRegMonitorClass::RegMonThread(LPVOID lParam)
{
    CRegMonitorClass* pThis = (CRegMonitorClass*)lParam;
    __try
    {
        pThis->RegMonFunction();
    }
    __except (GetExceptionCode() == EXCEPTION_ACCESS_VIOLATION)
    {
        assert(0);
    }
    return 0;
}
