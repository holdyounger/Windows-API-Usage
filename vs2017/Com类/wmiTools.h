
#pragma once
#include <iostream>
#include <WbemIdl.h>
#include <vector>
#include <string>
#include <atlstr.h>
using namespace std;
class CWmiIface
{
public:
	// 查询接口
	BOOL GetSingleItemInfo(__in const wstring &ClassName, __in const  wstring &ClassMember, __out wstring &chRetValue);
	BOOL GetGroupItemInfo(__in const wstring &ClassName, __in const vector<wstring> &vectClassMember, __out std::vector<wstring> &vecRetValue);
	void VariantToString(__in const LPVARIANT, __out CString &) const;
public:
	IEnumWbemClassObject*	m_pEnumClsObj;
	IWbemClassObject*		m_pWbemClsObj;
	IWbemServices*			m_pWbemSvc;
	IWbemLocator*			m_pWbemLoc;
	BOOL                    m_bInitialize;
public:
	// 只能获取硬件信息
	HRESULT InitWmi();
	HRESULT ReleaseWmi();
	CWmiIface(void);
	~CWmiIface(void);
	CWmiIface(const CWmiIface&);
	CWmiIface &operator=(const CWmiIface&);
};

