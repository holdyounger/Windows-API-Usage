#pragma once

#include <windows.h>
#include <tchar.h>
#include <stdio.h>


class CRegMonitorClass
{
public:
	//标准构析函数
	CRegMonitorClass();
	~CRegMonitorClass();

protected:
	HKEY   m_hMonRegKey; //准备监视的键句柄
	HANDLE m_hMonThread; //监控线程
	HANDLE m_hExitEvent; //线程退出事件
	HANDLE m_hMonEvent;  //监控变化事件
#define _DbgPrintf(x) printf(x)
public:
	virtual BOOL StopMonitor(); //停止监控
	virtual BOOL StartMonitor(HKEY hPrmKey, LPCTSTR szKey); //启动监控

protected:
	virtual DWORD RegMonFunction(); //监控注册表功能函数 
	static DWORD CALLBACK RegMonThread(LPVOID lParam); //监控线程
};