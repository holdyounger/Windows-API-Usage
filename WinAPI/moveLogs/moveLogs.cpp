#include <iostream>
#include <vector>
#include <string>
#include <windows.h>
#include <winbase.h>
#include <WinUser.h>
#include <strsafe.h>
#include <ShObjIdl.h>
#include <cstring>
#include <shlobj.h>
#include <comutil.h>  
#include <io.h>
#include <Shlwapi.h>
#pragma comment(lib, "Shlwapi.lib") 
#pragma comment(lib, "comsuppw.lib")
#pragma comment(linker, "/entry:mainCRTStartup") 

bool _SetFileRebootDelete(const std::string& file_path) {
	if (MoveFileExA(file_path.c_str(), 0, MOVEFILE_DELAY_UNTIL_REBOOT)) {
		// MessageBoxA(NULL, "成功！", "设置成功！", MB_OK);
		return true;
	}
	else {
		// MessageBoxA(NULL, "失败！", "设置失败！", MB_OK);
		return false;
	}
}

void getFiles(std::string path, std::vector<std::string>& files, std::vector<std::string>& names)
{
	//文件句柄，win10用long long，win7用long就可以了
	long hFile = 0;
	//文件信息 
	struct _finddata_t fileinfo;
	std::string p;
	if ((hFile = _findfirst(p.assign(path).append("\\*").c_str(), &fileinfo)) != -1)
	{
		do
		{
			//如果是目录,迭代之 //如果不是,加入列表 
			if ((fileinfo.attrib & _A_SUBDIR))
			{
				if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
				{
					getFiles(p.assign(path).append("\\").append(fileinfo.name), files, names);
				}
			}
			else
			{
				files.push_back(p.assign(path).append("\\").append(fileinfo.name));
				names.push_back(fileinfo.name);
			}
		} while (_findnext(hFile, &fileinfo) == 0);
		_findclose(hFile);
	}
}


std::string GetLoactionDir()
{
	DWORD dwLen = 0;
	CHAR szAppDataDir[MAX_PATH];
	dwLen = ExpandEnvironmentStringsA("%programdata%", szAppDataDir, MAX_PATH);

	if (dwLen != 0) {
		StringCchCatA(szAppDataDir, sizeof(szAppDataDir), ("\\TrustAgent\\logs"));
	}

	return szAppDataDir;
}

void DeleteInstallPackage()
{
	TCHAR szTempPath[MAX_PATH] = {};
	ExpandEnvironmentStrings(L"%TEMP%", szTempPath, MAX_PATH);

	if (!PathFileExists(szTempPath))
		return;

	TCHAR szFindPath[MAX_PATH] = {};
	PathCombine(szFindPath, szTempPath, L"TrustAgent_ICBC_*.exe");

	WIN32_FIND_DATA FindFileData;
	HANDLE hFind = FindFirstFile(szFindPath, &FindFileData);
	if (hFind == INVALID_HANDLE_VALUE)
		return;

	do
	{
		TCHAR szFilePath[MAX_PATH] = {};
		PathCombine(szFilePath, szTempPath, FindFileData.cFileName);
		if (!DeleteFile(szFilePath))
		{
			MoveFileEx(szFilePath, NULL, MOVEFILE_DELAY_UNTIL_REBOOT);
		}
	} while (FindNextFile(hFind, &FindFileData));

	FindClose(hFind);
}

void DeleteAppstorePackage()
{
	TCHAR szTempPath[MAX_PATH] = {};
	PathAppend(szTempPath, L"D:\\gscdatas\\SoftButler");

	if (!PathFileExists(szTempPath))
		return;

	TCHAR szFindPath[MAX_PATH] = {};
	PathCombine(szFindPath, szTempPath, L"TrustAgent_ICBC_*.exe");

	WIN32_FIND_DATA FindFileData;
	HANDLE hFind = FindFirstFile(szFindPath, &FindFileData);
	if (hFind == INVALID_HANDLE_VALUE)
		return;

	do
	{
		TCHAR szFilePath[MAX_PATH] = {};
		PathCombine(szFilePath, szTempPath, FindFileData.cFileName);
		if (!DeleteFile(szFilePath))
		{
			MoveFileEx(szFilePath, NULL, MOVEFILE_DELAY_UNTIL_REBOOT);
		}
	} while (FindNextFile(hFind, &FindFileData));

	FindClose(hFind);
}

int main()
{	
	using namespace std;
	std::string filepath = GetLoactionDir();

	vector<string> files;
	vector<string> names;

	// 删除日志目录
	getFiles(filepath, files, names);
	for (int i = 0; i < files.size(); i++)
	{
		printf("-----========files:%d========---\n", i);
		// cout << "files" << i << ':' << files[i] << endl;

		// wstring wsName;
		// wsName = string2wstring(files[i]);
		cout << "lnkPath: " << files[i] << endl;

			// 删除文件
		if (DeleteFileA(files[i].c_str())) {
			_SetFileRebootDelete(files[i]);
		}
		else {
			DWORD error = GetLastError();
			MoveFileExA(files[i].c_str(), NULL, MOVEFILE_DELAY_UNTIL_REBOOT);
		}

	}
	

	// 删除升级Temp目录下的安装包
	DeleteInstallPackage();

	// 删除应用商店下载目录下的安装包
	DeleteAppstorePackage();

	return 0;
}