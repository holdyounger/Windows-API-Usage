// set 的使用demo

#include <iostream>
#include <set>
#include <vector>
#include "WlPortPool.h"

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
	
	WNTPORT::GetFreePort(stuPort);
	cout << "start port:" << stuPort.nStartPort << endl;
	cout << "range:" << stuPort.nRange << endl;
	cout << "Enable:" << stuPort.bEnable << endl;

	stuPort.bEnable = false;
	WNTPORT::SetPortIsAvailable(stuPort);
}