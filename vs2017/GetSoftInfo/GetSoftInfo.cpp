#include<string>
#include<iostream>
#include<Windows.h>
#include<io.h>
#include<tchar.h>
using namespace std;
#pragma warning(disable:4996)
#pragma comment(lib, "version.lib")

std::string GetSoftVersion(const char* exepath)
{
	PCWSTR filePath = L"D:\\Program Files\\Typora\\Typora.exe";
	std::string r = "";
	if (!exepath)
		return r;
	if (_access(exepath, 0) != 0)
		return r;
	UINT sz = GetFileVersionInfoSize(L"D:\\Program Files\\Typora\\Typora.exe", 0);
	int d = GetLastError();
	if (sz != 0) {
		r.resize(sz, 0);
		char *pBuf = NULL;
		pBuf = new char[sz];
		VS_FIXEDFILEINFO *pVsInfo;
		if (GetFileVersionInfo((LPCWSTR)filePath, 0, sz, pBuf)) {
			if (VerQueryValue(pBuf, (LPCWSTR)"\\", (void **)&pVsInfo, &sz)) {
				sprintf(pBuf, "%d.%d.%d.%d", HIWORD(pVsInfo->dwFileVersionMS), LOWORD(pVsInfo->dwFileVersionMS), HIWORD(pVsInfo->dwFileVersionLS), LOWORD(pVsInfo->dwFileVersionLS));
				r = pBuf;
			}
		}
		delete pBuf;
	}
	return r;
}

int main()
{

	std::string info = GetSoftVersion("D:\\Program Files\\Typora\\Typora.exe");
	std::cout << info << std::endl;
	system("pause");
	return 0;
}