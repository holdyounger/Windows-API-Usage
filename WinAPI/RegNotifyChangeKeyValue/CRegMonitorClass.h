#pragma once

#include <windows.h>
#include <tchar.h>
#include <stdio.h>


class CRegMonitorClass
{
public:
	//��׼��������
	CRegMonitorClass();
	~CRegMonitorClass();

protected:
	HKEY   m_hMonRegKey; //׼�����ӵļ����
	HANDLE m_hMonThread; //����߳�
	HANDLE m_hExitEvent; //�߳��˳��¼�
	HANDLE m_hMonEvent;  //��ر仯�¼�
#define _DbgPrintf(x) printf(x)
public:
	virtual BOOL StopMonitor(); //ֹͣ���
	virtual BOOL StartMonitor(HKEY hPrmKey, LPCTSTR szKey); //�������

protected:
	virtual DWORD RegMonFunction(); //���ע����ܺ��� 
	static DWORD CALLBACK RegMonThread(LPVOID lParam); //����߳�
};