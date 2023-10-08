#include <Windows.h>
#include <iostream>


int main(int argc, TCHAR* argv[])
{
	HKEY hKey;
	LPCTSTR lpSubKey = L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run";

	DWORD samDesire = KEY_READ | KEY_WOW64_64KEY; //KEY_READ | KEY_WOW64_32KEY   HKEY_LOCAL_MACHINE\SOFTWARE\Wow6432Node\Microsoft\Windows\CurrentVersion\Run

	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, lpSubKey, 0, samDesire, &hKey) != ERROR_SUCCESS)
	{
		return 1;
	}

	TCHAR szValueName[MAX_PATH] = { 0 };
	BYTE szKeyName[MAX_PATH] = { 0 };
	LONG lStatus;
	DWORD dwIndex = 0;
	DWORD dwSizeValueName = MAX_PATH;
	DWORD dwSizeofKeyName = MAX_PATH;
	DWORD dwType;

	dwSizeValueName = MAX_PATH;
	dwSizeofKeyName = MAX_PATH;
	do
	{
		lStatus = RegEnumValue(hKey, dwIndex++, szValueName, &dwSizeValueName,
			NULL, &dwType, szKeyName, &dwSizeofKeyName);
		if (lStatus == ERROR_SUCCESS)
		{
			wprintf(L"Key Name = %s\n", (wchar_t *)szValueName);

		}

		dwSizeValueName = MAX_PATH;
		dwSizeofKeyName = MAX_PATH;

	} while (lStatus != ERROR_NO_MORE_ITEMS);

	RegCloseKey(hKey);

	system("pause");

	return 0;
}