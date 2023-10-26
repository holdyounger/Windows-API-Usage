#pragma once
#include <atlstr.h>
#include <windows.h>
#include <iostream>
#include <vector>

using namespace std;

class InjectDll
{
	enum ID_TYPE
	{
		TYPE_NAME = 0,
		TYPE_PID
	};

public:

	InjectDll(CString process, CString DllPath);
	InjectDll(DWORD pid, CString DllPath);

	void SetCheckEnvValue(CString name, CString value = "") { m_envName = name; m_envValue = value; };

	BOOL Inject();

private:



private:
	CString m_strProcess;
	CString m_strDllPath;
	DWORD m_pid;
	ID_TYPE m_type;

	CString m_envName;
	CString m_envValue;
};

