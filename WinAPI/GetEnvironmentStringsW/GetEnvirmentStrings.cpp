#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <atlstr.h>
#include <shlwapi.h>
#include <winternl.h>


BOOL IsEnvValue()
{
    BOOL bMySQL = FALSE;
    const WCHAR MYSQL_ENV_SERVICE_STR[] = L"ALLUSERSPROFILE";
    const WCHAR MYSQL_ENV_PID_STR[] = L"MYSQLD_PARENT_PID";

    CString strValue = "";
    LPTSTR lpszVariable;
    LPTCH lpvEnv;

    //获得环境变量内存块的指针
    lpvEnv = GetEnvironmentStrings();

    if (lpvEnv == NULL)
    {
        printf("GetEnvironmentStrins failed(%d)/n", GetLastError());

        goto _END_;
    }

    //环境变量字符串是以NULL分隔的，内存块以NULL结尾
    lpszVariable = (LPTSTR)lpvEnv;
    while (*lpszVariable != L'\0')
    {
        printf("[%ws] env str : %ws\n", __FUNCTIONW__, lpszVariable);

        int i = _ARRAYSIZE(MYSQL_ENV_SERVICE_STR);

        if (StrCmpNIW(MYSQL_ENV_SERVICE_STR, lpszVariable, _ARRAYSIZE(MYSQL_ENV_SERVICE_STR)-1) == 0)
        {
            strValue = lpszVariable + _ARRAYSIZE(MYSQL_ENV_SERVICE_STR);
            strValue.Trim();
            printf("[%ws] target : %ws, value : %ws", __FUNCTIONW__, MYSQL_ENV_SERVICE_STR, strValue);
            break;
        }

        lpszVariable += lstrlen(lpszVariable) + 1;   //移动指针
    }

_END_:
    if (!lpvEnv)
    {
        FreeEnvironmentStrings(lpvEnv);
    }
    return bMySQL;
}

void GetEnvByUseApi()
{
    LPTSTR lpszVariable;
    LPTCH lpvEnv;

    //获得环境变量内存块的指针
    lpvEnv = GetEnvironmentStrings();

    if (lpvEnv == NULL)
    {
        printf("GetEnvironmentStrins failed(%d)/n", GetLastError());

        return ;
    }

    //环境变量字符串是以NULL分隔的，内存块以NULL结尾
    lpszVariable = (LPTSTR)lpvEnv;
    while (*lpszVariable != L'\0')
    {
        _tprintf(TEXT("%s\n"), lpszVariable);

        lpszVariable += lstrlen(lpszVariable) + 1;   //移动指针
    }

    printf("----------\n");

    lpszVariable = (LPTSTR)lpvEnv;
    while (*lpszVariable != L'\0') {
        printf("[%ws] env str : %ws\n", __FUNCTIONW__, lpszVariable);

        lpszVariable += wcslen(lpszVariable) + 1;
    }

    FreeEnvironmentStrings(lpvEnv);
}

// 使用 RtlQueryEnvironmentVariable_U 获取环境变量

typedef BOOL(WINAPI* LPFN_GLPI)(PSYSTEM_LOGICAL_PROCESSOR_INFORMATION, PDWORD);
typedef NTSTATUS (WINAPI* RTQUERY_ENV_VALUE)( 
    IN PVOID Environment OPTIONAL,
    IN PWSTR      Name,
    IN SIZE_T      NameLength,
    OUT PWSTR      value,
    OUT SIZE_T      ValueLength,
    OUT PSIZE_T     ReturnLength);
void GetEnvByNtDll()
{
    RTQUERY_ENV_VALUE rtev;
    rtev = (RTQUERY_ENV_VALUE)GetProcAddress(
        GetModuleHandle(TEXT("ntdll")),
        "RtlQueryEnvironmentVariable");
    if (NULL == rtev)
    {
        printf("rtev Get Failed");
    }
    else
    {
        WCHAR buf[] = L"ALLUSERSPROFILE";//定义宽字节字符串
        WCHAR value[MAX_PATH];
        SIZE_T valueLength = 0;
        SIZE_T returnLength = 0;

        NTSTATUS status;

        status = rtev(NULL, buf, _ARRAYSIZE(buf), value, valueLength, &returnLength);


        if (NT_SUCCESS(status))
        {
            printf("%ws", value);
        }

    }
}

int main()
{
    IsEnvValue();

    system("pause");

    return 1;

}