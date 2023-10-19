/*************************************************************************************************
*
* File Name    : WMIInof.h
*
* Created      : 2023/10/19
*
* Author       : holdyounger
*
* Model        :
*
* Description  : simple wmi execquery
*
**************************************************************************************************/
#ifndef _WMIINFO_H_
#define _WMIINFO_H_

#include <WbemIdl.h>
#include <atlstr.h>
#pragma comment(lib,"WbemUuid.lib")

class CWmiInfo
{
public:
    CWmiInfo();
    ~CWmiInfo();

public:
    HRESULT InitWmi();    //��ʼ��WMI
    HRESULT ReleaseWmi(); //�ͷ�

    /*��ȡһ�����Ա
    *@param [in ] ClassName   Example: "Win32_Processor"
    *@param [in ] ClassMember Example: "SerialNumber"
    *@param [out] chRetValue
    *@param return TRUE success; false fail

    Example:
    CString strRetValue;
    GetSingleItemInfo(_T("Win32_Processor"),_T("Caption"),strRetValue);
    */
    BOOL GetSingleItemInfo(CString ClassName, CString ClassMember, CString& chRetValue);

    /*��ȡһ����Ķ����Ա
   *@param [in ] ClassName   Example: "Win32_Processor"
   *@param [in ] ClassMember Example: "SerialNumber"
   *@param [in ] n   ��Ա����
   *@param [out] chRetValue
   *@param return TRUE success; false fail

   Example:
   CString strRetValue;CString [] strClassMem = {_T("Caption"),_T("CurrentClockSpeed"),_T("DeviceID"),_T("Manufacturer"),_T("Manufacturer")};
   GetGroupItemInfo(_T("Win32_Processor"),strClassMem,5,strRetValue);
   */
    BOOL GetGroupItemInfo(CString ClassName, CString ClassMember[], int n, CString& chRetValue);

private:
    void VariantToString(const LPVARIANT, CString&) const;//��Variant���͵ı���ת��ΪCString

private:
    IEnumWbemClassObject* m_pEnumClsObj;
    IWbemClassObject* m_pWbemClsObj;
    IWbemServices* m_pWbemSvc;
    IWbemLocator* m_pWbemLoc;
};
#endif