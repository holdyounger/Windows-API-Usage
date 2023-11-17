#include "ComHelper.h"

#include <comdef.h>
#include <Wbemidl.h>

#pragma comment(lib, "wbemuuid.lib")

DWORD CComHelper::GetServicePidByName(LPCWSTR lpszServiceName)
{
    DWORD dwPID = 0;

    BOOL bUninit = TRUE;
    HRESULT hres = CoInitializeEx(0, COINIT_MULTITHREADED);
    printf("[CComHelper::GetServicePidByName] hres : %08x", hres);
    if (FAILED(hres))
    {
        bUninit = FALSE;
    }

    IEnumWbemClassObject* pEnumerator = NULL;
    CString szQueryStr;
    szQueryStr.AppendFormat(L"select ProcessID from win32_Service where Name='%s'", lpszServiceName);
    printf("[CComHelper::GetServicePidByName] WQL : %s", szQueryStr);
    hres = ExecQuery(szQueryStr, &pEnumerator);
    if (SUCCEEDED(hres) && pEnumerator)
    {
        IWbemClassObject *pclsObj = NULL;
        do
        {
            ULONG uReturn = 0;
            HRESULT hr = pEnumerator->Next(WBEM_INFINITE, 1, &pclsObj, &uReturn);

            printf("[CComHelper::ExecQuery] pEnumerator->Next uReturn : %d", uReturn);
            if (0 == uReturn || pclsObj == NULL)
            {
                break;
            }

            VARIANT vtProp;

            hr = pclsObj->Get(L"ProcessID", 0, &vtProp, 0, 0);
            if (SUCCEEDED(hr) && vtProp.vt == VT_I4)
            {
                dwPID = vtProp.intVal;
            }
            VariantClear(&vtProp);
            pclsObj->Release();
        } while (FALSE);

        pEnumerator->Release();
    }

    if (bUninit)
    {
        CoUninitialize();
    }
    
    return dwPID;
}

DWORD CComHelper::GetServicePidByNameWild(LPCWSTR lpszServiceName)
{
    DWORD dwPID = 0;

    BOOL bUninit = TRUE;
    HRESULT hres = CoInitializeEx(0, COINIT_MULTITHREADED);
    printf("[CComHelper::GetServicePidByName] hres : %08x", hres);
    if (FAILED(hres))
    {
        bUninit = FALSE;
    }

    IEnumWbemClassObject* pEnumerator = NULL;
    CString szQueryStr;
    szQueryStr.AppendFormat(L"select ProcessID from win32_Service where Name like '%s'", lpszServiceName);
    printf("[CComHelper::GetServicePidByName] WQL : %s", szQueryStr);
    hres = ExecQuery(szQueryStr, &pEnumerator);
    if (SUCCEEDED(hres) && pEnumerator)
    {
        IWbemClassObject *pclsObj = NULL;
        do
        {
            ULONG uReturn = 0;
            HRESULT hr = pEnumerator->Next(WBEM_INFINITE, 1, &pclsObj, &uReturn);

            printf("[CComHelper::ExecQuery] pEnumerator->Next uReturn : %d", uReturn);
            if (0 == uReturn || pclsObj == NULL)
            {
                break;
            }

            VARIANT vtProp;

            hr = pclsObj->Get(L"ProcessID", 0, &vtProp, 0, 0);
            if (SUCCEEDED(hr) && vtProp.vt == VT_I4)
            {
                dwPID = vtProp.intVal;
            }
            VariantClear(&vtProp);
            pclsObj->Release();
        } while (FALSE);

        pEnumerator->Release();
    }

    if (bUninit)
    {
        CoUninitialize();
    }

    return dwPID;
}

DWORD CComHelper::GetProcessInfoByName(LPCWSTR lpszProcessName, CString & sProcessPath)
{
    DWORD dwPID = 0;

    BOOL bUninit = TRUE;
    HRESULT hres = CoInitializeEx(0, COINIT_MULTITHREADED);
    printf("[CComHelper::GetServicePidByName] hres : %08x", hres);
    if (FAILED(hres))
    {
        bUninit = FALSE;
    }

    IEnumWbemClassObject* pEnumerator = NULL;
    CString szQueryStr;
    szQueryStr.AppendFormat(L"select ProcessID,ExecutablePath from win32_Process where Name='%s'", lpszProcessName);
    printf("[CComHelper::GetServicePidByName] WQL : %s", szQueryStr);
    hres = ExecQuery(szQueryStr, &pEnumerator);
    if (SUCCEEDED(hres) && pEnumerator)
    {
        IWbemClassObject *pclsObj = NULL;
        do
        {
            ULONG uReturn = 0;
            HRESULT hr = pEnumerator->Next(WBEM_INFINITE, 1, &pclsObj, &uReturn);

            printf("[CComHelper::ExecQuery] pEnumerator->Next uReturn : %d", uReturn);
            if (0 == uReturn || pclsObj == NULL)
            {
                break;
            }

            VARIANT vtProp;

            hr = pclsObj->Get(L"ProcessID", 0, &vtProp, 0, 0);
            if (SUCCEEDED(hr) && vtProp.vt == VT_I4)
            {
                dwPID = vtProp.intVal;
            }
            VariantClear(&vtProp);

            VARIANT vtExecutePath;
            hr = pclsObj->Get(L"ExecutablePath", 0, &vtExecutePath, 0, 0);
            if (SUCCEEDED(hr) && vtExecutePath.vt == VT_BSTR)
            {
                sProcessPath = vtExecutePath.bstrVal;
                VariantClear(&vtExecutePath);
            }

            pclsObj->Release();
        } while (FALSE);

        pEnumerator->Release();
    }

    if (bUninit)
    {
        CoUninitialize();
    }

    return dwPID;
}

DWORD CComHelper::GetProcessInfoByNameWild(LPCWSTR lpszProcessName, CString & sProcessPath)
{
    DWORD dwPID = 0;

    BOOL bUninit = TRUE;
    HRESULT hres = CoInitializeEx(0, COINIT_MULTITHREADED);
    printf("[CComHelper::GetServicePidByName] hres : %08x", hres);
    if (FAILED(hres))
    {
        bUninit = FALSE;
    }

    IEnumWbemClassObject* pEnumerator = NULL;
    CString szQueryStr;
    szQueryStr.AppendFormat(L"select ProcessID,ExecutablePath from win32_Process where Name like '%s'", lpszProcessName);
    printf("[CComHelper::GetServicePidByName] WQL : %s", szQueryStr);
    hres = ExecQuery(szQueryStr, &pEnumerator);
    if (SUCCEEDED(hres) && pEnumerator)
    {
        IWbemClassObject *pclsObj = NULL;
        do
        {
            ULONG uReturn = 0;
            HRESULT hr = pEnumerator->Next(WBEM_INFINITE, 1, &pclsObj, &uReturn);

            printf("[CComHelper::ExecQuery] pEnumerator->Next uReturn : %d", uReturn);
            if (0 == uReturn || pclsObj == NULL)
            {
                break;
            }

            VARIANT vtProp;

            hr = pclsObj->Get(L"ProcessID", 0, &vtProp, 0, 0);
            if (SUCCEEDED(hr) && vtProp.vt == VT_I4)
            {
                dwPID = vtProp.intVal;
            }
            VariantClear(&vtProp);

            VARIANT vtExecutePath;
            hr = pclsObj->Get(L"ExecutablePath", 0, &vtExecutePath, 0, 0);
            if (SUCCEEDED(hr) && vtExecutePath.vt == VT_BSTR)
            {
                sProcessPath = vtExecutePath.bstrVal;
                VariantClear(&vtExecutePath);
            }

            pclsObj->Release();
        } while (FALSE);

        pEnumerator->Release();
    }

    if (bUninit)
    {
        CoUninitialize();
    }

    return dwPID;
}

DWORD CComHelper::GetProcessInfoByNameEx(LPCWSTR lpszProcessName, WP_PROCESS_INFO_EX & info)
{
    DWORD dwPID = 0;

    BOOL bUninit = TRUE;
    HRESULT hres = CoInitializeEx(0, COINIT_MULTITHREADED);
    printf("[%s] hres : %08x", __FUNCTIONW__, hres);
    if (FAILED(hres))
    {
        bUninit = FALSE;
    }

    IEnumWbemClassObject* pEnumerator = NULL;
    CString szQueryStr;
    szQueryStr.AppendFormat(L"select ProcessID,ExecutablePath,CommandLine,ParentProcessId from win32_Process where Name='%s'", lpszProcessName);
    printf("[%s] WQL : %s", __FUNCTIONW__, szQueryStr);
    hres = ExecQuery(szQueryStr, &pEnumerator);
    if (SUCCEEDED(hres) && pEnumerator)
    {
        IWbemClassObject *pclsObj = NULL;
        HRESULT    hr = WBEM_S_NO_ERROR;

        while (WBEM_S_NO_ERROR == hr)
        {
            ULONG uReturn = 0;
            hr = pEnumerator->Next(WBEM_INFINITE, 1, &pclsObj, &uReturn);

            printf("[%s] pEnumerator->Next uReturn : %d", __FUNCTION__, uReturn);
            if (0 == uReturn || pclsObj == NULL)
            {
                break;
            }

            VARIANT vtProp;

            hr = pclsObj->Get(L"ProcessID", 0, &vtProp, 0, 0);
            VariantInit(&vtProp);
            if (SUCCEEDED(hr) && vtProp.vt == VT_I4)
            {
                info.dwPID = vtProp.intVal;
            }
            VariantClear(&vtProp);

            VARIANT vtExecutePath;
            VariantInit(&vtExecutePath);
            hr = pclsObj->Get(L"ExecutablePath", 0, &vtExecutePath, 0, 0);
            if (SUCCEEDED(hr) && vtExecutePath.vt == VT_BSTR)
            {
                info.sProcessPath = vtExecutePath.bstrVal;
            }
            VariantClear(&vtExecutePath);

            VARIANT vtCommandLine;
            VariantInit(&vtCommandLine);
            hr = pclsObj->Get(L"CommandLine", 0, &vtCommandLine, 0, 0);
            if (SUCCEEDED(hr) && vtCommandLine.vt == VT_BSTR)
            {
                info.sCommandLine = vtCommandLine.bstrVal;
            }
            VariantClear(&vtCommandLine);

            VARIANT vtParentProcessId;
            VariantInit(&vtParentProcessId);
            hr = pclsObj->Get(L"ParentProcessId", 0, &vtParentProcessId, 0, 0);
            if (SUCCEEDED(hr) && vtParentProcessId.vt == VT_I4)
            {
                info.dwParentPID = vtParentProcessId.intVal;
            }
            VariantClear(&vtParentProcessId);

            pclsObj->Release();

        }

        pEnumerator->Release();
    }

    if (bUninit)
    {
        CoUninitialize();
    }

    return dwPID;
}

DWORD CComHelper::GetProcessCommandLineByName(LPCWSTR lpszProcessName, CString & sCommandLine, CString sKeyWord)
{
    DWORD dwPID = 0;

    BOOL bUninit = TRUE;
    HRESULT hres = CoInitializeEx(0, COINIT_MULTITHREADED);
    printf("[%s] hres : %08x", __FUNCTIONW__, hres);
    if (FAILED(hres))
    {
        bUninit = FALSE;
    }

    IEnumWbemClassObject* pEnumerator = NULL;
    CString szQueryStr;
    szQueryStr.AppendFormat(L"select ProcessID,CommandLine from win32_Process where Name='%s'", lpszProcessName);
    printf("[%s] WQL : %s", __FUNCTIONW__, szQueryStr);
    hres = ExecQuery(szQueryStr, &pEnumerator);
    if (SUCCEEDED(hres) && pEnumerator)
    {
        IWbemClassObject *pclsObj = NULL;
        do
        {
            ULONG uReturn = 0;
            HRESULT hr = pEnumerator->Next(WBEM_INFINITE, 1, &pclsObj, &uReturn);

            printf("[%s] pEnumerator->Next uReturn : %d", __FUNCTIONW__, uReturn);
            if (0 == uReturn || pclsObj == NULL)
            {
                break;
            }

            VARIANT vtProp;

            hr = pclsObj->Get(L"ProcessID", 0, &vtProp, 0, 0);
            if (SUCCEEDED(hr) && vtProp.vt == VT_I4)
            {
                dwPID = vtProp.intVal;
            }
            VariantClear(&vtProp);

            VARIANT vtCommandLine;
            hr = pclsObj->Get(L"CommandLine", 0, &vtCommandLine, 0, 0);
            if (SUCCEEDED(hr) && vtCommandLine.vt == VT_BSTR)
            {
                sCommandLine = vtCommandLine.bstrVal;
                VariantClear(&vtCommandLine);
            }

            pclsObj->Release();

            if (sCommandLine.Find(sKeyWord) != -1)
            {
                break;
            }

        } while (TRUE);

        pEnumerator->Release();
    }

    if (bUninit)
    {
        CoUninitialize();
    }

    return dwPID;
}

HRESULT CComHelper::ExecQuery(LPCWSTR qStr, IEnumWbemClassObject** pOutEnumerator)
{
    HRESULT hres = S_OK;

    IWbemServices *pSvc = NULL;
    IWbemLocator *pLoc = NULL;
    do
    {
        hres = CoCreateInstance(CLSID_WbemLocator, 0, CLSCTX_INPROC_SERVER, IID_IWbemLocator, (LPVOID *)&pLoc);
        if (FAILED(hres))
        {
            printf("[CComHelper::ExecQuery] CoCreateInstance hres : %08x", hres);
            break;
        }

        hres = pLoc->ConnectServer(_bstr_t(L"ROOT\\CIMV2"), NULL, NULL, 0, NULL, 0, 0, &pSvc);
        if (FAILED(hres))
        {
            printf("[CComHelper::ExecQuery] ConnectServer hres : %08x", hres);
            break;
        }

        hres = CoSetProxyBlanket(pSvc, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, NULL,
            RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE);
        if (FAILED(hres))
        {
            printf("[CComHelper::ExecQuery] CoSetProxyBlanket hres : %08x", hres);
            break;
        }

        hres = pSvc->ExecQuery(
            bstr_t("WQL"),
            bstr_t(qStr),
            WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
            NULL,
            pOutEnumerator);

        if (FAILED(hres))
        {
            printf("[CComHelper::ExecQuery] ExecQuery hres : %08x", hres);
            break;
        }
    } while (FALSE);

    if (pSvc)
    {
        pSvc->Release();
    }

    if (pLoc)
    {
        pLoc->Release();
    }

    return hres;
}
