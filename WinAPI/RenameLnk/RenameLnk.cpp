// RenameLnk.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <shlobj.h>
#include <io.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#pragma comment(lib, "shell32.lib")
using namespace std;

string TCHARTurnString(TCHAR* STR)
{

	int iLen = WideCharToMultiByte(CP_ACP, 0, STR, -1, NULL, 0, NULL, NULL);


	char* chRtn = new char[iLen * sizeof(char)];

	WideCharToMultiByte(CP_ACP, 0, STR, -1, chRtn, iLen, NULL, NULL);

	string str(chRtn);

	return str;

}

string getDesktopPath()
{
	std::cout << "Hello World!\n";

	TCHAR path[MAX_PATH];
	//获取当前用户的桌面路径
	SHGetSpecialFolderPath(0, path, CSIDL_DESKTOPDIRECTORY, 0);

	wcout << path << endl;

	return TCHARTurnString(path);
}

wstring string2wstring(string str)
{
	wstring result;
	//获取缓冲区大小，并申请空间，缓冲区大小按字符计算  
	int len = MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.size(), NULL, 0);
	TCHAR* buffer = new TCHAR[len + 1];
	//多字节编码转换成宽字节编码  
	MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.size(), buffer, len);
	buffer[len] = '\0';             //添加字符串结尾  
	//删除缓冲区并返回值  
	result.append(buffer);
	delete[] buffer;
	return result;
}

bool CrnModifyShortcutArguments(string strLnkFile, string strArguments)
{
	wchar_t wszBuf[MAX_PATH];
	char szBuf[MAX_PATH];
	wstring wsPath;
	wstring wsParm;
	wsPath = string2wstring(strLnkFile);
	wsParm = string2wstring(strArguments);

	if ( INVALID_FILE_ATTRIBUTES == GetFileAttributes(wsPath.c_str()))
	{
		::MessageBox(0, L"指定的快捷方式未找到!", L"错误", MB_OK | MB_ICONERROR);
		return false;
	}

	bool bResult = (CoInitialize(NULL) == S_OK);
	if (bResult)
	{
		IShellLink* pShellLink;
		bResult = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER,
			IID_IShellLink, (void**)&pShellLink) >= 0;
		if (bResult)
		{
			IPersistFile* ppf;
			bResult = pShellLink->QueryInterface(IID_IPersistFile, (void**)&ppf) >= 0;

			if (bResult)
			{
				bResult = ppf->Load(wsPath.c_str(), TRUE) >= 0;
				if (bResult)
				{
					// 63 63 72 75 6E 2E 63 6F 6D
					pShellLink->SetArguments(wsParm.c_str());
					pShellLink->SetDescription(wsParm.c_str());
					bResult = ppf->Save(wsPath.c_str(), TRUE) >= 0;
				}
				ppf->Release();
			}
			pShellLink->Release();
		}
		CoUninitialize();
	}

	return bResult;
}
int main()
{
	string strDeskTopPath = getDesktopPath();
	string strLnkFile = strDeskTopPath + string("\\StopTiqi.lnk");
	string strFile = strDeskTopPath + string("\\奇安信零信任.lnk");


	rename(strLnkFile.c_str(), strFile.c_str());

	// bool bResult = CrnModifyShortcutArguments(strLnkFile, "www.ccrun.com");
	// cout << bResult << endl;
}
