#include <windows.h>
#include <tchar.h>
#include <string>
#include <stdio.h>
#include <stdio.h>
#include <iostream>
#define MAX 128
#define TOTALBYTES    8192
#define BYTEINCREMENT 4096
using namespace std;

void queryValueEx(HKEY key)
{
	DWORD BufferSize = TOTALBYTES;
	DWORD cbData;
	DWORD dwRet = ERROR_MORE_DATA;
	DWORD dwType;

	WCHAR szLocation[MAX_PATH] = { '\0' };
	PPERF_DATA_BLOCK PerfData = (PPERF_DATA_BLOCK)malloc(BufferSize);
	cbData = BufferSize;

	while (dwRet == ERROR_MORE_DATA)
	{
		// Get a buffer that is big enough.

		BufferSize += BYTEINCREMENT;
		PerfData = (PPERF_DATA_BLOCK)realloc(PerfData, BufferSize);
		cbData = BufferSize;

		printf(".");
		dwRet = RegQueryValueEx(HKEY_LOCAL_MACHINE,
			TEXT("appmodel"),
			NULL,
			NULL,
			(LPBYTE)PerfData,
			&cbData);
	}
	if (dwRet == ERROR_SUCCESS)
		printf("\n\nFinal buffer size is %d\n", BufferSize);
	else printf("\nRegQueryValueEx failed (%d)\n", dwRet);
}

void quertInfo()
{
	HKEY key;
	// HKEY_CURRENT_USER\SOFTWARE\csvconverter
	if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, "SOFTWARE\\Corsair\\Corsair Utility Engine", 0, KEY_ENUMERATE_SUB_KEYS | KEY_QUERY_VALUE, &key) == ERROR_SUCCESS)
	{
	}
	else {
		OutputDebugStringA("RegOpenKeyEx !=ERROR_SUCCESS");
	}

	DWORD dwIndex = 0, lpcchClass = 0, lpReserved = 0, lpcbMaxClassLen = 0, NameCnt = 0, MaxNameLen = 0, Type = 0;
	DWORD KeySize = 0, KeyCnt = 0, KeyMaxLen = 0, DateSize = 0, MaxDateLen = 0;

	if (RegQueryInfoKey(key, NULL, NULL, NULL, &KeyCnt, &KeyMaxLen, &lpcbMaxClassLen, &NameCnt, &MaxNameLen, &MaxDateLen, NULL, NULL) != ERROR_SUCCESS)
	{
		OutputDebugStringA("RegQueryInfoKey Error \n");
	}
	printf("KeyCnt:%d\r\n", KeyCnt);
	printf("KeyMaxLen:%d\r\n", KeyMaxLen);
	printf("NameCnt:%d\r\n", NameCnt);
	printf("MaxNameLen:%d\r\n", MaxNameLen);
	printf("lpcchClass:%d\r\n", lpcchClass);
	printf("MaxDateLen:%d\r\n", MaxDateLen);
	
}
int main(int argc, char **argv)
{
	HKEY hKey;
	WCHAR szLocation[MAX_PATH] = { '\0' };
	DWORD dwSize = sizeof(DWORD);
	DWORD dwType = REG_SZ;
	LPCTSTR studioPath = TEXT("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Svchost");
	LONG ret;
	ret = RegOpenKeyEx(HKEY_LOCAL_MACHINE, studioPath, 0, KEY_READ, &hKey);
	wprintf(L"RegOpenKeyEx returns %d\n", ret);

	if (ERROR_SUCCESS == ret)
	{
		ret = RegQueryValueEx(hKey, TEXT("BITSgroup"), 0, &dwType, NULL, &dwSize);
		wprintf(L"RegQueryValueEx returns %d, dwType=%d\n", ret, dwType);

		ret = RegQueryValueEx(hKey, TEXT("BITSgroup"), 0, &dwType, (LPBYTE)&szLocation, &dwSize);
		wprintf(L"RegQueryValueEx returns %d, dwType=%d\n", ret, dwType);
		queryValueEx(hKey);
		if (ERROR_SUCCESS == ret)
		{
			wprintf(L"Location: %s\n", szLocation);
		}
		RegCloseKey(hKey);
	}
	return 0;
}