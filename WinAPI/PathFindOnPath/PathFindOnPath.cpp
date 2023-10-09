// 通过正则方式查找文件
#include <Windows.h>
#include <iostream>

#include <shlwapi.h>

#pragma comment(lib,"Shlwapi.lib")

int main() {
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