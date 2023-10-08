#include <windows.h>
#include <stdio.h>
#include <atlstr.h>
#include <vector>
#include <iostream>
using namespace std;

#pragma warning(disable:4996)


void main(int argc, char* argv[])
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
	vector<wstring> keyName;

	int nCount = 0;
	do
	{
		nError = RegEnumValueW(hKey, dwIndex++, szValueName, &dwSizeValueName, NULL, &dwType, szKeyName, &dwSizeofKeyName);
		if (nError == ERROR_SUCCESS)
		{
			wprintf(L"Key Name = %s\n", (wchar_t *)szValueName);
			//wprintf(L"Key Name = %s\n", (wchar_t *)szKeyName);
			wchar_t wsStr[128];
			_wcsncpy_l(wsStr, (wchar_t *)szValueName,sizeof(szValueName)/sizeof(szValueName[0]),0);
			keyName.push_back(wsStr);

			//VALENT pVal;
			//pVal.ve_valuename = (LPWSTR)wsStr.c_str();//输入值项名称
		}

		dwSizeValueName = MAX_PATH;
		dwSizeofKeyName = MAX_PATH;

	} while (nError != ERROR_NO_MORE_ITEMS);

	for (nCount = 0; nCount <  keyName.size(); nCount++)
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