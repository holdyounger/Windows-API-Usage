#pragma once
#include <atlstr.h>
#include <wbemcli.h>

typedef struct _WP_PROCESS_INFO_EX
{
    DWORD       dwPID;
    DWORD       dwParentPID;
    CString     sProcessPath;
    CString     sCommandLine;
}WP_PROCESS_INFO_EX;

class CComHelper
{
public:
    static DWORD GetServicePidByName(LPCWSTR lpszServiceName);
    static DWORD GetServicePidByNameWild(LPCWSTR lpszServiceName);
    static DWORD GetProcessInfoByName(LPCWSTR lpszProcessName, CString &sProcessPath);
    static DWORD GetProcessInfoByNameWild(LPCWSTR lpszProcessName, CString &sProcessPath);

    static DWORD GetProcessInfoByNameEx(LPCWSTR lpszProcessName, WP_PROCESS_INFO_EX &info);

    static DWORD GetProcessCommandLineByName(LPCWSTR lpszProcessName, CString &sCommandLine, CString sKeyWord);

private:
    static HRESULT ExecQuery(LPCWSTR qStr, IEnumWbemClassObject** pOutEnumerator);
};

