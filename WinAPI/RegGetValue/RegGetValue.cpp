#include <windows.h>
#include <stdio.h>
#include <atlstr.h>
#include <vector>
#include <iostream>
#include <string>
using namespace std;
#define MAX_KEY_LENGTH 255
#define MAX_VALUE_NAME 16383

#pragma warning(disable:4996)

TCHAR pszSubPath[MAX_VALUE_NAME] = _T("SOFTWARE\\Wow6432Node\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Microsoft EdgeWebView");

#define REGPATH _T("SOFTWARE\\Wow6432Node\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Microsoft EdgeWebView")

#define REG_APP_PATH		 _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\App Paths\\TrustAgent.exe")
#define REG_UNINST_KEY		 _T("Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\TrustAgent")

//获取同步信息，即从注册表寻找对应的键值
BOOL GetSynchroInfo(const TCHAR* pszSub)
{
	HKEY hKey;
	TCHAR szLocation[MAX_PATH] = { '\0' };
	DWORD dwSize = sizeof(DWORD);
	DWORD dwIndex = 0;
	DWORD dwType = REG_SZ;
	LONG lRet = RegOpenKeyEx(HKEY_LOCAL_MACHINE, pszSub, 0, KEY_READ, &hKey);



	if (ERROR_SUCCESS != lRet)
	{
		//assert(0);
		return FALSE;
	}

	lRet = RegQueryValueEx(hKey, TEXT("InstallLocation"), 0, &dwType, NULL, &dwSize);
	lRet = RegQueryValueEx(hKey, TEXT("InstallLocation"), NULL, &dwType, (LPBYTE)&szLocation, &dwSize);
	wprintf(L"RegQueryValueEx returns %d, dwSize=%d\n", lRet, dwSize);
	//找不到
	if (ERROR_SUCCESS != lRet)
	{
		// 
		printf("Not Fund\n");
	}

	TCHAR* pszpath = szLocation;

	printf("查询结果：%S\n", pszpath);

	if (_tcsclen(szLocation))
	{		
		cout << "键值信息: " << *pszpath << endl;
		if (*pszpath != 2 && *pszpath != 1)
			cout << "运行状态" << endl;
		if (*pszpath == 3 )
			cout << "运行状态" << endl;
		return TRUE;
	}
	RegCloseKey(hKey);
	return false;
}

std::string GetSynchroInfo()
{
	#define TRUSTAGENT "TrustAgent.exe"
	USES_CONVERSION;

	HKEY hKey;
	TCHAR szLocation[MAX_PATH] = { '\0' };
	DWORD dwSize = sizeof(DWORD);
	DWORD dwIndex = 0;
	DWORD dwType = REG_SZ;
	LONG lRet = RegOpenKeyEx(HKEY_LOCAL_MACHINE, REG_APP_PATH, 0, KEY_READ, &hKey);

	if (ERROR_SUCCESS != lRet)
	{
		//assert(0);
		printf("Cant Find\n");
		return FALSE;
	}

	lRet = RegQueryValueEx(hKey, TEXT(""), 0, &dwType, NULL, &dwSize);
	lRet = RegQueryValueEx(hKey, TEXT(""), NULL, &dwType, (LPBYTE)&szLocation, &dwSize);
	wprintf(L"RegQueryValueEx returns %d, dwSize=%d\n", lRet, dwSize);
	//找不到
	if (ERROR_SUCCESS != lRet)
	{
		// 
		printf("Not Fund\n");
	}

	printf("查询结果：%S\n", szLocation);

	CString strCov;
	strCov.Format(_T("%s"), szLocation);


	std::string strRet = W2A(strCov.GetBuffer());

	strRet.append("\\").append(TRUSTAGENT);

	return strRet;

	RegCloseKey(hKey);
	return nullptr;
}


vector<wstring> keyName;

void EnumListValue()
{
	HKEY hKey = NULL;

	//KEY_READ | KEY_WOW64_32KEY   HKEY_LOCAL_MACHINE\SOFTWARE\Wow6432Node\Microsoft\Windows\CurrentVersion\Run
	DWORD samDesire = KEY_READ | KEY_WOW64_64KEY;
	LPCTSTR lpSubKey = L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run";

	if (RegOpenKeyExW(HKEY_LOCAL_MACHINE, lpSubKey, 0, samDesire, &hKey) != ERROR_SUCCESS)
	{
		return;
	}

	FILETIME timeFile;
	long nError = 0;
	VALENT pValent[MAX_PATH];
	DWORD nValentCount = MAX_PATH;
	memset(pValent, 0, sizeof(pValent));

	// 1.获取子项键名
	DWORD dwSizeValueName = MAX_PATH;
	DWORD dwSizeofKeyName = MAX_PATH;
	DWORD dwIndex = 0;
	DWORD dwType;
	TCHAR szValueName[MAX_PATH] = { 0 };
	BYTE szKeyName[MAX_PATH] = { 0 };

	int nCount = 0;
	do
	{
		nError = RegEnumValueW(hKey, dwIndex++, szValueName, &dwSizeValueName, NULL, &dwType, szKeyName, &dwSizeofKeyName);
		if (nError == ERROR_SUCCESS)
		{
			wprintf(L"Key Name = %s\n", (wchar_t*)szValueName);
			//wprintf(L"Key Name = %s\n", (wchar_t *)szKeyName);
			wchar_t wsStr[1024];
			_wcsncpy_l(wsStr, (wchar_t*)szValueName, sizeof(szValueName) / sizeof(szValueName[0]), 0);
			keyName.push_back(wsStr);

			//VALENT pVal;
			//pVal.ve_valuename = (LPWSTR)wsStr.c_str();//输入值项名称
		}

		dwSizeValueName = MAX_PATH;
		dwSizeofKeyName = MAX_PATH;

	} while (nError != ERROR_NO_MORE_ITEMS);

	for (nCount = 0; nCount < keyName.size(); nCount++)
	{
		pValent[nCount].ve_valuename = (LPWSTR)keyName[nCount].c_str();
	}

	// 2.根据键名获取值
	{
		char szVale[1024];
		DWORD nValueLen = 1024;

		printf("2.根据键名获取值 Success!\n");

		if (ERROR_SUCCESS == (nError = RegQueryMultipleValues(hKey, pValent, nCount, (LPWSTR)szVale, &nValueLen)))
		{

			printf("RegQueryMultipleValues Success!\n");

			CString szResult;
			printf("------------------------------------------\n");
			for (int i = 0; i < nCount; ++i)
			{

				CString szTemp;
				wcout.imbue(std::locale("chs"));

				/*if (REG_SZ == pVal.ve_type)
				{

					szTemp.Format(_T("第%d个参数[%s]的值是:%s\r"), i,
						pVal.ve_valuename, (char*)pVal.ve_valueptr);

					wcout << "s=" << szTemp.GetString() << endl;
				}
				else if (REG_DWORD == pVal.ve_type)
				{
					szTemp.Format(_T("第%d个参数[%s]的值是:%d\r"), i,
						pVal.ve_valuename, *((DWORD*)pVal.ve_valueptr));

					wcout << "s=" << szTemp.GetString() << endl;
				}*/



				if (REG_SZ == pValent[i].ve_type)
				{

					szTemp.Format(_T("第%d个参数[%s]的值是:%s\r"), i,
						pValent[i].ve_valuename, (char*)pValent[i].ve_valueptr);

					wcout << "s=" << szTemp.GetString() << endl;
				}
				else if (REG_DWORD == pValent[i].ve_type)
				{
					szTemp.Format(_T("第%d个参数[%s]的值是:%d\r"), i,
						pValent[i].ve_valuename, *((DWORD*)pValent[i].ve_valueptr));

					wcout << "s=" << szTemp.GetString() << endl;
				}


				if (!szTemp.IsEmpty())

					szResult += szTemp;
			}
			printf("------------------------------------------\n");

		}
		else
		{
			printf("erroe code: %d", nError);
		}


		// Close registry keys.
		if (hKey)
			RegCloseKey(hKey);

	}
}

void main(int argc, char* argv[])
{
	
	std::string strRet;
	EnumListValue();
	keyName;

	GetSynchroInfo();
	// printf("查询结果：%s\n", strRet.c_str());
	return;
}