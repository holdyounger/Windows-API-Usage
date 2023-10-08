// GetProcess.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <windows.h>
#include <tlhelp32.h>	//进程快照函数头文件
#include <stdio.h>

int main()
{
	int countProcess = 0;									//当前进程数量计数变量
	PROCESSENTRY32W currentProcess;						//存放快照进程信息的一个结构体
	currentProcess.dwSize = sizeof(currentProcess);		//在使用这个结构之前，先设置它的大小
	HANDLE hProcess = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);//给系统内的所有进程拍一个快照

	if (hProcess == INVALID_HANDLE_VALUE)
	{
		printf("CreateToolhelp32Snapshot()调用失败!\n");
		return -1;
	}

	bool bMore = Process32First(hProcess, &currentProcess);	//获取第一个进程信息
	while (bMore)
	{
		printf("PID=%5u    PName= %ls \n", currentProcess.th32ProcessID, currentProcess.szExeFile);	//遍历进程快照，轮流显示每个进程信息
		bMore = Process32Next(hProcess, &currentProcess);	//遍历下一个
		countProcess++;
	}

	CloseHandle(hProcess);	//清除hProcess句柄
	printf("共有以上%d个进程在运行\n", countProcess);
	system("pause");
	return 0;
}
// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
