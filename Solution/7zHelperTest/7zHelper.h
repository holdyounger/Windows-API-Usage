#pragma once
#include <atlstr.h>
#include <vector>
using namespace std;

//Usage:  Demo代码
//vector<CString>   vecFileList;
//CString   strArchive=L"d:\\test\\a.7z";
//CString  strFile1 = L"d:\\a.txt";
//CString  strFile2 = L"d:\\b.txt";
//vecFileList.push_back(strFile1);
//vecFileList.push_back(strFile2);
//Q7zComprocess(strArchive,vecFileList);
//vecFileList.clear();

//返回值0 表示成功
//strArchive 7z包绝对路径
//vetFileList容器是压缩文件的绝对路径
//目前只支持压缩到顶层目录
int Q7zComprocess(CString  strArchive, CString strPassword, vector<CString> &vetFileList);