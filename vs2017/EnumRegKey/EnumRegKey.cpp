
#include <iostream>
#include <stdio.h>
using namespace std;

// QueryKey - Enumerates the subkeys of key and its associated values.

// hKey - Key whose subkeys and values are to be enumerated.

#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <string>
using namespace std;
#define MAX_KEY_LENGTH 255
#define MAX_VALUE_NAME 16383

TCHAR pszSubPath[MAX_VALUE_NAME] = _T("SYSTEM\\CurrentControlSet\\Services");

//获取同步信息，即从注册表寻找对应的键值
BOOL GetSynchroInfo(TCHAR* pszSubPath)
{
	HKEY hKey;
	TCHAR szLocation[MAX_PATH] = { '\0' };
	DWORD dwSize = sizeof(DWORD);
	DWORD dwIndex = 0;
	DWORD dwType = REG_SZ;
	LONG lRet = RegOpenKeyEx(HKEY_LOCAL_MACHINE, pszSubPath, 0, KEY_READ, &hKey);



	if (ERROR_SUCCESS != lRet)
	{
		//assert(0);
		return FALSE;
	}

	lRet = RegQueryValueEx(hKey, TEXT("Start"), 0, &dwType, NULL, &dwSize);
	lRet = RegQueryValueEx(hKey, TEXT("Start"), NULL, &dwType, (LPBYTE)&szLocation, &dwSize);
	wprintf(L"RegQueryValueEx returns %d, dwSize=%d\n", lRet, dwSize);
	//找不到
	if (ERROR_SUCCESS != lRet)
	{
		// 
		printf("Not Fund\n");
	}

	TCHAR *pszpath = szLocation;
	if (_tcsclen(szLocation))
	{
		cout << "键值信息: " << *pszpath << endl;
		if (*pszpath != 2 && *pszpath != 1)
			cout << "运行状态" << endl;
		return TRUE;
	}
	RegCloseKey(hKey);
	return false;
}

TCHAR szLocation[MAX_PATH] = { '\0' };

void QueryKey(HKEY hKey)
{

	WCHAR    achKey[MAX_KEY_LENGTH];   // buffer for subkey name
	DWORD    cbName;                   // size of name string
	WCHAR    achClass[MAX_PATH] = TEXT("");  // buffer for class name
	DWORD    cchClassName = MAX_PATH;  // size of class string
	DWORD    cSubKeys = 0;               // number of subkeys
	DWORD    cbMaxSubKey;              // longest subkey size
	DWORD    cchMaxClass;              // longest class string
	DWORD    cValues;              // number of values for key
	DWORD    cchMaxValue;          // longest value name
	DWORD    cbMaxValueData;       // longest value data
	DWORD    cbSecurityDescriptor; // size of security descriptor
	FILETIME ftLastWriteTime;      // last write time
	DWORD i, retCode;
	LONG lRet;
	TCHAR achValue[MAX_VALUE_NAME];
	DWORD cchValue = MAX_VALUE_NAME;
	DWORD dwRegType = REG_SZ;
	DWORD dwSize = sizeof(DWORD);

	// Get the class name and the value count.

	retCode = RegQueryInfoKey(

		hKey,                    // key handle
		achClass,                // buffer for class name
		&cchClassName,           // size of class string
		NULL,                    // reserved
		&cSubKeys,               // number of subkeys
		&cbMaxSubKey,            // longest subkey size
		&cchMaxClass,            // longest class string
		&cValues,                // number of values for this key
		&cchMaxValue,            // longest value name
		&cbMaxValueData,         // longest value data
		&cbSecurityDescriptor,   // security descriptor
		&ftLastWriteTime);       // last write time

   // Enumerate the subkeys, until RegEnumKeyEx fails.
	if (cSubKeys)
	{
		printf("\nNumber of subkeys: %d\n", cSubKeys);

		for (i = 0; i < cSubKeys; i++)
		{

			_tcscpy_s(pszSubPath, _T("SYSTEM\\CurrentControlSet\\Services\\"));

			cbName = MAX_KEY_LENGTH;

			retCode = RegEnumKeyEx(hKey, i,
				achKey,
				&cbName,
				NULL,
				NULL,
				NULL,
				&ftLastWriteTime);

			if (retCode == ERROR_SUCCESS)
			{

				_tprintf(TEXT("(%d) %s\n"), i + 1, achKey);

				_tcscat_s(pszSubPath, achKey);

				GetSynchroInfo(pszSubPath);

			}
		}
	}
}

void _tmain(void)
{

	HKEY hTestKey;
	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE,
		TEXT("SYSTEM\\CurrentControlSet\\Services"),
		0,
		KEY_READ,
		&hTestKey) == ERROR_SUCCESS
		)
	{
		QueryKey(hTestKey);
	}

}