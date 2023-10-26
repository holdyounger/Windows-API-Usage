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

//ֹͣ���
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

//�������
//�ɹ��Ƿ���TURE
BOOL CRegMonitorClass::StartMonitor(HKEY hPrmKey, LPCTSTR szKey)
{
    BOOL bRet = FALSE;
    StopMonitor();

    do
    {
        //��ע���
        DWORD dwRet = RegOpenKeyEx(hPrmKey, szKey, 0, KEY_ALL_ACCESS, &m_hMonRegKey);
        if (dwRet != ERROR_SUCCESS || m_hMonRegKey == NULL)
        {
            _DbgPrintf(("RegOpenKeyEx failed %u\n"), dwRet);
            bRet = -1;
            break;
        }

        //�����˳��¼�
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

        //������ر仯�¼�
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

        //��������߳�
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

    //ʧ��ʱ�رռ��
    if (bRet <= 0)
    {
        StopMonitor();
    }

    return bRet;
}

//���ע����ܺ���
DWORD CRegMonitorClass::RegMonFunction()
{
    assert(m_hMonEvent);
    assert(m_hMonRegKey);
    assert(m_hExitEvent);

    BOOL bEixtFlag = FALSE;
    while ((!bEixtFlag))
    {
        //�����¼�
        ResetEvent(m_hMonEvent);

        //ע���¼�
        DWORD dwRet = RegNotifyChangeKeyValue(m_hMonRegKey,
            TRUE, REG_LEGAL_CHANGE_FILTER, m_hMonEvent, TRUE);

        if (dwRet != ERROR_SUCCESS)
        {
            TRACE(("RegNotifyChangeKeyValue failed %u\n"), dwRet);
            break;
        }

        //�ȴ��¼�
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
                //������Ӵ���

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

//����߳�
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
