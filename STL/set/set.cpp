// set.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
// Set.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include "vld.h"
#include <set>
#include <vector>
using namespace std;

struct man	 
{
	int age;
	int height;
	bool isContain(int i)
	{
		if (this->age == i)
		{
			return true;
		}
		return false;
	}
};


void testSet()
{
	std::cout << "Hello World!\n";

	set<int> setAge;

	for (int i = 0; i < 10; i++)
	{
		setAge.insert(i);
	}
	set<int>::iterator it = setAge.begin();
	for (; it != setAge.end(); it++)
	{
		cout << *it << " ";
	}
	if (setAge.end() != setAge.find(10))
	{
		cout << "找到了" << endl;
	}
	else
	{
		cout << "没找到" << endl;
	}

	STU_PORT_POOL stuDynamicPort;
	int nPortsNum = 0;
	stuDynamicPort.nStartPort = 50;
	stuDynamicPort.nRange = 20;
	set<int> setMan;
	for (int i = 0; i < 100; i++)
	{
		setMan.insert(i);
	}

	it = setMan.begin();
	for (; it != setMan.end(); it++)
	{
		if (stuDynamicPort.isContain(*it))
		{
			nPortsNum++;
		}
	}
	cout << "总共有" << nPortsNum << "个数字符合条件" << endl;
}

void testVector()
{
	vector<man> mans;

	for (int i = 0; i < 10; i++)
	{
		man aMen;
		aMen.age = i + 1;
		aMen.height = i + 170;

		mans.push_back(aMen);
	}

	vector<man>::iterator it = mans.begin();
	for (; it != mans.end(); it++)
	{
		if (it->isContain((int)1))
		{
			cout << "11111111111111111" << endl;
		}
		cout << it->age << ' ' << it->height << endl;
	}

	it = mans.begin();
	for (; it != mans.end(); it++)
	{
		if (it->age >= 6)
		{
			it = mans.erase(it);
			it--;
		}
	}
	cout << "---------------" << endl;


	it = mans.begin();
	for (; it != mans.end(); it++)
	{
		cout << it->age << ' ' << it->height << endl;
	}

	cout << "---------------" << endl;
	man newMan = *(mans.begin() + 3);
	cout << newMan.age << ' ' << newMan.height << endl;

}


int main()
{
	int num = 0;
	STU_PORT_POOL stuPort;

	if (NO_ERROR == WNTPORT::getPortPoolStatus())
	{
		cout << "num:" << num << endl;
	}
	else
	{
		cout << "获取失败"<< endl;
	}
	
	WNTPORT::getPort(stuPort);
	cout << "start port:" << stuPort.nStartPort << endl;
	cout << "range:" << stuPort.nRange << endl;
	cout << "Enable:" << stuPort.bEnable << endl;

	stuPort.bEnable = false;
	WNTPORT::SetPortIsAvailable(stuPort);
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
