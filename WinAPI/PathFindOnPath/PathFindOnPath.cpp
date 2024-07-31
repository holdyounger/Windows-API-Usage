// 通过正则方式查找文件
#include <Windows.h>
#include <atlstr.h>
#include <iostream>

#include <shlwapi.h>

#pragma comment(lib,"Shlwapi.lib")

void GetTempDir()
{
    UINT uRetVal = 0;
    TCHAR lpTempPathBuffer[MAX_PATH];
    DWORD dwRetVal = 0;

    dwRetVal = GetTempPath(MAX_PATH,          // length of the buffer
        lpTempPathBuffer); // buffer for path 
    if (dwRetVal > MAX_PATH || (dwRetVal == 0))
    {
        printf(("GetTempPath failed"));
    }
}

static inline void GetNewFileName(const CString& path, __out CString& name) {
    int nSaveIdx = 1;
    CString cfgValue = L"dbPath: C:\\Program Files\\MongoDB\\Server\\7.0\\data";

    CString dataPath = cfgValue.Right(cfgValue.GetLength() - cfgValue.Find(L"dbPath:"));
    CString dataPath1 = cfgValue.GetString()+CString(L"dbPath: ").GetLength();

    CString sTempZipFilePath = path;
    CString sRetName;
    CString strFileName = path.Right(path.GetLength() - path.ReverseFind('\\') - 1);
    CString strZipFileName = strFileName.Left(strFileName.ReverseFind(L'.'));
    CString sExt = name.Right(name.GetLength() - name.ReverseFind('.') - 1);
    CString sFileName = name.Left(name.ReverseFind('.'));

    sTempZipFilePath.AppendFormat(L"%s", name);
    while (PathFileExistsW(sTempZipFilePath))
    {
        sRetName.Format(L"%s(%d).%s", sFileName, nSaveIdx++, sExt);

        // 压缩文件
        sTempZipFilePath = path;
        sTempZipFilePath.Append(sRetName);
    }

    name = sRetName;
}

int main() 
{
    CString name = "future.exe";
    GetNewFileName("C:\\rdbg\\123.exe", name);

    GetTempDir();

    // 定义文件查找句柄和查找条件  
    WIN32_FIND_DATAA findData;
    HANDLE hFind = INVALID_HANDLE_VALUE;
    LPCSTR lpPath = "D:\\Documents\\B_Tools\\"; // 查找路径，这里以C盘根目录为例  
    LPCSTR lpPattern = "*user*"; // 查找模式，这里匹配所有文件  

    char buffer_1[MAX_PATH] = "";
    char* lpStr1;
    lpStr1 = buffer_1;

    PathCombineA(lpStr1, lpPath, lpPattern);

    // 调用FindFirstFile函数开始查找文件  
    hFind = FindFirstFileA(lpStr1, &findData);
    if (hFind == INVALID_HANDLE_VALUE) {
        std::cout << "FindFirstFile failed with error: " << GetLastError() << std::endl;
        return 1;
    }

    // 循环遍历查找结果并输出文件名  
    do {
        std::cout << findData.cFileName << std::endl;
    } while (FindNextFileA(hFind, &findData));

    // 关闭文件查找句柄  
    FindClose(hFind);

    return 0;
}