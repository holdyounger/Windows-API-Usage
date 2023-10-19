#include "CWmiInfo.h"

CWmiInfo::CWmiInfo(void)
{
    m_pWbemSvc = NULL;
    m_pWbemLoc = NULL;
    m_pEnumClsObj = NULL;
}

CWmiInfo::~CWmiInfo(void)
{
    m_pWbemSvc = NULL;
    m_pWbemLoc = NULL;
    m_pEnumClsObj = NULL;
}

HRESULT CWmiInfo::InitWmi()
{
    HRESULT hr;

    //一、初始化COM组件
    //初始化COM
    hr = ::CoInitializeEx(0, COINIT_MULTITHREADED);
    if (SUCCEEDED(hr) || RPC_E_CHANGED_MODE == hr)
    {
        //设置进程的安全级别，（调用COM组件时在初始化COM之后要调用CoInitializeSecurity设置进程安全级别，否则会被系统识别为病毒）
        hr = CoInitializeSecurity(NULL,
            -1,
            NULL,
            NULL,
            RPC_C_AUTHN_LEVEL_PKT,
            RPC_C_IMP_LEVEL_IMPERSONATE,
            NULL,
            EOAC_NONE,
            NULL);
        //VERIFY(SUCCEEDED(hr));

        //二、创建一个WMI命名空间连接
        //创建一个CLSID_WbemLocator对象
        hr = CoCreateInstance(CLSID_WbemLocator,
            0,
            CLSCTX_INPROC_SERVER,
            IID_IWbemLocator,
            (LPVOID*)&m_pWbemLoc);
        //        VERIFY(SUCCEEDED(hr));

                //使用m_pWbemLoc连接到"root\cimv2"并设置m_pWbemSvc的指针
        hr = m_pWbemLoc->ConnectServer(CComBSTR(L"ROOT\\CIMV2"),
            NULL,
            NULL,
            0,
            NULL,
            0,
            0,
            &m_pWbemSvc);
        //        VERIFY(SUCCEEDED(hr));

                //三、设置WMI连接的安全性
        hr = CoSetProxyBlanket(m_pWbemSvc,
            RPC_C_AUTHN_WINNT,
            RPC_C_AUTHZ_NONE,
            NULL,
            RPC_C_AUTHN_LEVEL_CALL,
            RPC_C_IMP_LEVEL_IMPERSONATE,
            NULL,
            EOAC_NONE);
        //        VERIFY(SUCCEEDED(hr));

    }
    return(hr);
}

HRESULT CWmiInfo::ReleaseWmi()
{
    HRESULT hr;

    if (NULL != m_pWbemSvc)
    {
        hr = m_pWbemSvc->Release();
    }
    if (NULL != m_pWbemLoc)
    {
        hr = m_pWbemLoc->Release();
    }
    if (NULL != m_pEnumClsObj)
    {
        hr = m_pEnumClsObj->Release();
    }

    ::CoUninitialize();

    return(hr);
}

BOOL CWmiInfo::GetSingleItemInfo(CString ClassName, CString ClassMember, CString& chRetValue)
{
    USES_CONVERSION;

    CComBSTR query("SELECT * FROM ");
    VARIANT vtProp;
    ULONG uReturn;
    HRESULT hr;
    BOOL bRet = FALSE;

    if (NULL != m_pWbemSvc)
    {
        //查询类ClassName中的所有字段,保存到m_pEnumClsObj中
        query += CComBSTR(ClassName);
        hr = m_pWbemSvc->ExecQuery(CComBSTR("WQL"), query, WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
            0, &m_pEnumClsObj);
        if (SUCCEEDED(hr))
        {
            //初始化vtProp值
            VariantInit(&vtProp);
            uReturn = 0;

            //返回从当前位置起的第一个对象到m_pWbemClsObj中
            hr = m_pEnumClsObj->Next(WBEM_INFINITE, 1, &m_pWbemClsObj, &uReturn);
            if (SUCCEEDED(hr) && uReturn > 0)
            {
                //从m_pWbemClsObj中找出ClassMember标识的成员属性值,并保存到vtProp变量中
                hr = m_pWbemClsObj->Get(CComBSTR(ClassMember), 0, &vtProp, 0, 0);
                if (SUCCEEDED(hr))
                {
                    VariantToString(&vtProp, chRetValue);
                    VariantClear(&vtProp);//清空vtProp
                    bRet = TRUE;
                }
            }
        }
    }
    if (NULL != m_pEnumClsObj)
    {
        hr = m_pEnumClsObj->Release();
        m_pEnumClsObj = NULL;
    }
    if (NULL != m_pWbemClsObj)
    {
        hr = m_pWbemClsObj->Release();
        m_pWbemClsObj = NULL;
    }
    return bRet;
}

BOOL CWmiInfo::GetGroupItemInfo(CString ClassName, CString ClassMember[], int n, CString& chRetValue)
{
    USES_CONVERSION;

    CComBSTR query("SELECT * FROM ");
    CString result, info;
    VARIANT vtProp;
    ULONG uReturn;
    HRESULT hr;
    int i;
    BOOL bRet = FALSE;
    if (NULL != m_pWbemSvc)
    {
        query += CComBSTR(ClassName);
        hr = m_pWbemSvc->ExecQuery(CComBSTR("WQL"), query, WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, 0, &m_pEnumClsObj);
        if (SUCCEEDED(hr))
        {
            VariantInit(&vtProp); //初始化vtProp变量
            if (m_pEnumClsObj)
            {
                Sleep(10);
                uReturn = 0;
                hr = m_pEnumClsObj->Next(WBEM_INFINITE, 1, &m_pWbemClsObj, &uReturn);
                if (SUCCEEDED(hr) && uReturn > 0)
                {
                    for (i = 0; i < n; ++i)
                    {
                        hr = m_pWbemClsObj->Get(CComBSTR(ClassMember[i]), 0, &vtProp, 0, 0);
                        if (SUCCEEDED(hr))
                        {
                            VariantToString(&vtProp, info);
                            chRetValue += info + _T("\t");
                            VariantClear(&vtProp);
                            bRet = TRUE;
                        }
                    }
                    chRetValue += _T("\r\n");
                }
            }
        }
    }

    if (NULL != m_pEnumClsObj)
    {
        hr = m_pEnumClsObj->Release();
        m_pEnumClsObj = NULL;
    }
    if (NULL != m_pWbemClsObj)
    {
        hr = m_pWbemClsObj->Release();
        m_pWbemClsObj = NULL;
    }
    return bRet;
}

void CWmiInfo::VariantToString(const LPVARIANT pVar, CString& chRetValue) const
{
    USES_CONVERSION;

    CComBSTR HUGEP* pBstr;
    BYTE HUGEP* pBuf;
    LONG low, high, i;
    HRESULT hr;

    switch (pVar->vt)
    {
    case VT_BSTR:
    {
        chRetValue = W2T(pVar->bstrVal);
    }
    break;
    case VT_BOOL:
    {
        if (VARIANT_TRUE == pVar->boolVal)
            chRetValue = "是";
        else
            chRetValue = "否";
    }
    break;
    case VT_I4:
    {
        chRetValue.Format(_T("%d"), pVar->lVal);
    }
    break;
    case VT_UI1:
    {
        chRetValue.Format(_T("%d"), pVar->bVal);
    }
    break;
    case VT_UI4:
    {
        chRetValue.Format(_T("%d"), pVar->ulVal);
    }
    break;

    case VT_BSTR | VT_ARRAY:
    {
        hr = SafeArrayAccessData(pVar->parray, (void HUGEP**) & pBstr);
        hr = SafeArrayUnaccessData(pVar->parray);
        chRetValue = W2T(pBstr->m_str);
    }
    break;

    case VT_I4 | VT_ARRAY:
    {
        SafeArrayGetLBound(pVar->parray, 1, &low);
        SafeArrayGetUBound(pVar->parray, 1, &high);

        hr = SafeArrayAccessData(pVar->parray, (void HUGEP**) & pBuf);
        hr = SafeArrayUnaccessData(pVar->parray);
        CString strTmp;
        high = min(high, MAX_PATH * 2 - 1);
        for (i = low; i <= high; ++i)
        {
            strTmp.Format(_T("%02X"), pBuf[i]);
            chRetValue += strTmp;
        }
    }
    break;
    default:
        break;
    }
}