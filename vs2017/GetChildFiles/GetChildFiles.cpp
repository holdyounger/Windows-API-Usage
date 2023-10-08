// TestForOnlyC++.cpp : 定义控制台应用程序的入口点。
//


#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <windows.h>
#include <tchar.h>
#include<io.h>
using namespace std;
void getAllFiles(string path, vector<string>& files)
{
	//文件句柄  
	long   hFile = 0;
	//文件信息  
	struct _finddata_t fileinfo;  //很少用的文件信息读取结构
	string p;  //string类很有意思的一个赋值函数:assign()，有很多重载版本
	if ((hFile = _findfirst(p.assign(path).append("\\*.*").c_str(), &fileinfo)) != -1)
	{
		do
		{
			if ((fileinfo.attrib &  _A_SUBDIR))  //判断是否为文件夹
			{
				if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
				{
					files.push_back(p.assign(path).append("/").append(fileinfo.name));//保存文件夹名字
					getAllFiles(p.assign(path).append("/").append(fileinfo.name), files);//递归当前文件夹
				}
			}
			else    //文件处理
			{
				files.push_back(p.assign(path).append("/").append(fileinfo.name));//文件名
			}
		} while (_findnext(hFile, &fileinfo) == 0);  //寻找下一个，成功返回0，否则-1
		_findclose(hFile);
	}
	else
	{
		printf("Failed(%d)\n", GetLastError());
	}
}

//测试
int main(int argc, const char *argv[])
{
	SetThreadLocale(MAKELCID(MAKELANGID(LANG_CHINESE, SUBLANG_CHINESE_SIMPLIFIED), SORT_CHINESE_UNICODE));

	printf("_________Start Test_________");
	string DATA_DIR = "C:/Users/DELL/Desktop/中文测试";
	vector<string> files;
	//测试
	char DistAll[] = { "AllFiles.txt" };
	if (argc < 2)
	{
		printf("缺少参数");
		getAllFiles(DATA_DIR, files);//所有文件与文件夹的路径都输出
	}
	else
	{
		getAllFiles(argv[1], files);//所有文件与文件夹的路径都输出
	}
	
	//ofstream ofn(DistAll);  //输出文件流
	int size = files.size();
	int  FaiNum = 0;
	//ofn << size << endl;
	cout << "Get Files Nums:" << size << endl;
	for (int i = 0; i < size; i++)
	{
		//ofn << files[i] << endl;
		cout << files[i] << endl;
	}
	//ofn.close();
	return 0;
}
