// beginthread.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <windows.h>
#include <process.h>
#include <string.h>
#include <tchar.h>
using std::cout;
using std::endl;

typedef struct _STU_PEOPLE
{
	char szName[50];
	int age;
}STU_PEOPLE,*PSTU_PEOPLE;

void thread_1(void *)
{
	
	cout << "调用了线程 thread_1 无进入参数" << endl;
}

void thread_2(void *lpVoid)
{
	Sleep(10);
	int pnInt = (int)lpVoid;
	cout << "调用了线程 thread_2 进入参数为：" << pnInt << endl;
}

void thread_3(void *lpVoid)
{
	Sleep(50);
	PSTU_PEOPLE pnInt = (PSTU_PEOPLE)lpVoid;
	cout << "调用了线程 thread_3 进入参数为：" << pnInt->szName << ':' << pnInt->age << endl;
}


int main()
{
	STU_PEOPLE stuChild;

	char name[] = { "Child_LI" };
	strcpy_s(stuChild.szName, name);
	stuChild.age = 6;


    std::cout << "Start Thread!\n";
	_beginthread(thread_1, 0, NULL);
	_beginthread(thread_2, 0, (void*)stuChild.age);
	_beginthread(thread_3, 0, (void*)&stuChild);

	std::cout << "End Thread!\n";

	Sleep(100);
	system("pause");
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
