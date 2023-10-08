// WmicGetProcess.cpp : 定义控制台应用程序的入口点。
//

#include <Windows.h>
#include <tlhelp32.h>
#include <iostream>
using namespace std;

void getProcess()
{
	cout << __cplusplus << endl;
	system("pause");
	return;
	FILE *file;
	fopen_s(&file,"G:\\allprocess.txt","a+");
	TCHAR c[] = { "cmd.exe" }; //定义字符串并初始化,c为8长度,最后结尾有'\0',定义一个字符为'x',
	HANDLE handle; //定义CreateToolhelp32Snapshot系统快照句柄
	HANDLE handle1; //定义要结束进程句柄

	//获得系统快照句柄
	handle = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	PROCESSENTRY32 *info; //定义PROCESSENTRY32结构字指

	//PROCESSENTRY32 结构的 dwSize 成员设置成 sizeof(PROCESSENTRY32)
	info = new PROCESSENTRY32;
	info->dwSize = sizeof(PROCESSENTRY32);

	//调用一次 Process32First 函数，从快照中获取进程列表
	Process32First(handle, info);

	//重复调用 Process32Next，直到函数返回 FALSE 为止
	while (Process32Next(handle, info) != FALSE)
	{
		info->szExeFile; //指向进程名字
		wcout << info->szExeFile << endl;
		fprintf(file, "%ls", info->szExeFile);

		//strcmp字符串比较函数同要结束相同
		if (strcmp(c, info->szExeFile) == 0)
		{
			//MessageBox (NULL, info->szExeFile, TEXT ("HelloMsg"), 0);
			//PROCESS_TERMINATE表示为结束操作打开,FALSE=可继承,info->th32ProcessID=进程ID
			handle1 = OpenProcess(PROCESS_TERMINATE, FALSE, info->th32ProcessID);
			//结束进程
			TerminateProcess(handle1, 0);
		}
	}
	fclose( file );
}


int getProcess2()
{
	int countProcess=0;									//当前进程数量计数变量
	PROCESSENTRY32 currentProcess;						//存放快照进程信息的一个结构体
	currentProcess.dwSize = sizeof(currentProcess);		//在使用这个结构之前，先设置它的大小
	HANDLE hProcess = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);//给系统内的所有进程拍一个快照

	if (hProcess == INVALID_HANDLE_VALUE)
	{
		printf("CreateToolhelp32Snapshot()调用失败!\n");
		return -1;
	}

	bool bMore = Process32First(hProcess, &currentProcess);	//获取第一个进程信息
	while(bMore)
	{
		printf("PID=%5u    PName= %s", currentProcess.th32ProcessID, &currentProcess.szExeFile);	//遍历进程快照，轮流显示每个进程信息
		cout << &currentProcess.szExeFile << endl;
		bMore=Process32Next(hProcess, &currentProcess);	//遍历下一个
		countProcess++;
	}

	CloseHandle(hProcess);	//清除hProcess句柄
	printf("共有以上%d个进程在运行\n",countProcess);
	system("pause");

	return 0;
}
int main(int argc, char* argv[])
{
	getProcess2();
	return 0;
}

