// wsCmp.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <string>
using namespace std;
#define  test(p, Z) do { p.#Z = Z; } while(0)
#define one(var)  #var
#define _double(var) nVar##1
struct people {
	int age;
	string name;
};

int main()
{
	people p;
    std::cout << "Hello World!\n";
	string name = "Hello";
	cout << one(name) << endl;

	int nVar1 = 1;
	cout << "nvar##1 = (" << _double(1) << ')' << endl;

	wstring wstr1 = L"asddsa";
	wstring wstr2 = L"asddsa";
	cout << wstr1 == wstr2 << endl;

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
