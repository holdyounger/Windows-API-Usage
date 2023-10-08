// fileattribute.cpp: 定义控制台应用程序的入口点。
//
//#include "stdafx.h"
#include <string>
#include <iostream>
#include "attribute.h"  

#pragma comment(lib, "version")  	//这行必须要加上

using namespace std;

bool QueryValue(const std::string& ValueName, const std::string& szModuleName, std::string& RetStr)
{
	bool bSuccess = FALSE;
	BYTE*  m_lpVersionData = NULL;
	DWORD   m_dwLangCharset = 0;
	CHAR *tmpstr = NULL;

	do
	{
		if (!ValueName.size() || !szModuleName.size())
			break;

		DWORD dwHandle;
		// 判断系统能否检索到指定文件的版本信息
		DWORD dwDataSize = ::GetFileVersionInfoSizeA((LPCSTR)szModuleName.c_str(), &dwHandle);
		if (dwDataSize == 0)
			break;

		m_lpVersionData = new (std::nothrow) BYTE[dwDataSize];// 分配缓冲区
		if (NULL == m_lpVersionData)
			break;

		// 检索信息
		if (!::GetFileVersionInfoA((LPCSTR)szModuleName.c_str(), dwHandle, dwDataSize,
			(void*)m_lpVersionData))
			break;

		UINT nQuerySize;
		DWORD* pTransTable;
		// 设置语言
		if (!::VerQueryValueA(m_lpVersionData, "\\VarFileInfo\\Translation", (void **)&pTransTable, &nQuerySize))
			break;

		m_dwLangCharset = MAKELONG(HIWORD(pTransTable[0]), LOWORD(pTransTable[0]));
		if (m_lpVersionData == NULL)
			break;

		tmpstr = new (std::nothrow) CHAR[128];// 分配缓冲区
		if (NULL == tmpstr)
			break;
		sprintf_s(tmpstr, 128, "\\StringFileInfo\\%08lx\\%s", m_dwLangCharset, ValueName.c_str());
		LPVOID lpData;

		// 调用此函数查询前需要先依次调用函数GetFileVersionInfoSize和GetFileVersionInfo
		if (::VerQueryValueA((void *)m_lpVersionData, tmpstr, &lpData, &nQuerySize))
			RetStr = (char*)lpData;

		bSuccess = TRUE;
	} while (FALSE);

	// 销毁缓冲区
	if (m_lpVersionData)
	{
		delete[] m_lpVersionData;
		m_lpVersionData = NULL;
	}
	if (tmpstr)
	{
		delete[] tmpstr;
		tmpstr = NULL;
	}

	return bSuccess;
}

bool	BaseFlow::Attribute::GetFileDescription(const std::string& szModuleName, std::string& RetStr)
{
	return QueryValue("FileDescription", szModuleName, RetStr);
};   //获取文件说明

bool	BaseFlow::Attribute::GetFileVersion(const std::string& szModuleName, std::string& RetStr)
{
	return QueryValue("FileVersion", szModuleName, RetStr);
};	   //获取文件版本	

bool	BaseFlow::Attribute::GetInternalName(const std::string& szModuleName, std::string& RetStr)
{
	return QueryValue("InternalName", szModuleName, RetStr);
};	   //获取内部名称
bool	BaseFlow::Attribute::GetCompanyName(const std::string& szModuleName, std::string& RetStr)
{
	return QueryValue("CompanyName", szModuleName, RetStr);
};	   //获取公司名称
bool	BaseFlow::Attribute::GetLegalCopyright(const std::string& szModuleName, std::string& RetStr)
{
	return QueryValue("LegalCopyright", szModuleName, RetStr);
};    //获取版权
bool	BaseFlow::Attribute::GetOriginalFilename(const std::string& szModuleName, std::string& RetStr)
{
	return QueryValue("OriginalFilename", szModuleName, RetStr);
};  //获取原始文件名
bool	BaseFlow::Attribute::GetProductName(const std::string& szModuleName, std::string& RetStr)
{
	return QueryValue("ProductName", szModuleName, RetStr);
};	   //获取产品名称
bool	BaseFlow::Attribute::GetProductVersion(const std::string& szModuleName, std::string& RetStr)
{
	return QueryValue("ProductVersion", szModuleName, RetStr);
};    //获取产品版本
