#include <io.h>
#include <iostream>
#include <vector>
#include <string>
#include <Windows.h>
#include <ShObjIdl.h>
#include <cstring>
#include <shlobj.h>
#include <versionhelpers.h>
using namespace std;

#include <comutil.h>  
#pragma comment(lib, "comsuppw.lib")

string getLnkFormPath(wstring lnkPath)
{
	cout << "getLnkFromPath:";
	//wstring wstr2=UNICODE(lnkPath);
	wcout.imbue(locale("chs"));
	wcout<<lnkPath<<endl;
	cout << endl;

	LPCOLESTR str = lnkPath.c_str();

	// 初始化
	string sRet;
	char wRet[MAX_PATH];
	WIN32_FIND_DATA  fd;

	// 初始化 COM 库
	CoInitialize(NULL);
	IPersistFile* pPF = NULL;

	// 创建 COM 对象
	HRESULT hr = CoCreateInstance(
		CLSID_ShellLink,			// CLSID
		NULL,						// IUnknown 接口指针
		CLSCTX_INPROC_SERVER,		// CLSCTX_INPROC_SERVER：以 Dll 的方式操作类对象 
		IID_IPersistFile,			// COM 对象接口标识符
		(void**)(&pPF)				// 接收 COM 对象的指针
		);
		
	if(FAILED(hr)){cout << "CoCreateInstance failed." << endl;}

	// 判断是否支持接口
	IShellLink* pSL = NULL;
	hr = pPF->QueryInterface(
		IID_IShellLink,				// 接口 IID
		(void**)(&pSL)				// 接收指向这个接口函数虚标的指针
		);
	if(FAILED(hr)){cout << "QueryInterface failed." << endl;}

	// 打开文件
	hr = pPF->Load(
		str,					// 文件全路径
		STGM_READ					// 访问模式：只读
		);
	if(FAILED(hr)){cout << "Load failed ：" << GetLastError() << endl;}

	// 获取 Shell 链接来源
	hr = pSL->GetPath(wRet, MAX_PATH, &fd, 0);
	sRet = wRet;

	// 关闭 COM 库
	pPF->Release();
	CoUninitialize();

	return sRet;
} 


void getFiles(std::string path, std::vector<std::string>& files, std::vector<std::string>& names) 
{
	//文件句柄，win10用long long，win7用long就可以了
	long long hFile = 0;

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
		}while (_findnext(hFile, &fileinfo) == 0);
		_findclose(hFile); 
	} 
}


string wstring2string(const wstring& ws)
{
	_bstr_t t = ws.c_str();  
	char* pchar = (char*)t;  
	string result = pchar;  
	return result;  
}

wstring string2wstring(const string& s)
{
	_bstr_t t = s.c_str();  
	wchar_t* pwchar = (wchar_t*)t;  
	wstring result = pwchar;  
	return result; 
}

int main(int argc, char* argv[])
{
	string path = "C:\\ProgramData\\Microsoft\\Windows\\Start Menu\\Programs";
	vector<string> files;
	vector<string> names;

	getFiles(path,files,names);
 	for (int i = 0; i < files.size(); i++)
 	{
		printf("-----========files:%d========---\n",i);
 		// cout << "files" << i << ':' << files[i] << endl;

		wstring wsName;
		wsName = string2wstring(files[i]);	
		cout << "lnkPath: " << files[i] << endl;

		string wsDestPath = getLnkFormPath(wsName);
		cout << "destPath:";
		cout << wsDestPath;
		cout << endl;
 	}
 	cout  << "-------------------\n";
// 	for (int i = 0; i < names.size(); i++)
// 	{
// 		string path1 = "C:\\ProgramData\\Microsoft\\Windows\\Start Menu\\Programs\\";
// 		printf("-----========names:%d========---\n",i);
// 		cout << names[i] << endl;
// 		wstring wsName;
// 		wsName = string2wstring(path1.append(names[i]));	
// 		wcout << L"lnkPath: " << wsName << endl;
// 		// wprintf_s(L"lnkPath: %s\n", wsName.c_str());
// 		wstring wsDestPath = getLnkFormPath(wsName);
// 		wcout << L"destPath:" << wsDestPath << endl;
// 		// wprintf_s(L"destPath: %s\n", wsDestPath.c_str());
// 	}

	// QQ音乐.lnk
	// wstring wsDestPath = getLnkFormPath(L"C:\\ProgramData\\Microsoft\\Windows\\Start Menu\\Programs\\语雀.lnk");
	//printf("-----------------------------------\n%S\n", wsDestPath.c_str());
	// cout << "-----------------------------------\n" << wsDestPath.c_str() << endl;
	//system("pause");
	return 0;
}

