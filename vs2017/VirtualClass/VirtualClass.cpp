// VirtualClass.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <vld.h>

using namespace std;

class People
{
public:
	int Level;
	int age;
	//int height;
public:
	People() 
	{
		cout << "People init" << endl;
	}
	virtual ~People()
	{
		cout << "People end~" << endl;
	}
	virtual void GetLevel() = 0;
	/*{
		Level = NULL;
		cout << "People:" << Level << endl;
	}*/
	virtual void GetAge()
	{
		Level = NULL;
		cout << "People:" << Level << endl;
	}
};

class Proer : public People
{
public:
	Proer()
	{
		cout << "Proer init" << endl;
	}
	~Proer()
	{
		cout << "Proer end" << endl;
	}
	virtual void GetLevel()
	{
		cout << "Proer:" << Level << endl;
	}
};

class Teaer : public People
{
public:
	Teaer()
	{
		cout << "Teaer init" << endl;
	}
	~Teaer()
	{
		cout << "Teaer end" << endl;
	}
	virtual void GetLevel()
	{
		cout << "Teaer:" << Level << endl;
	}
};


int main()
{
	People *p = NULL;
	People *p1 = NULL;
	People *p2 = NULL;

	cout << sizeof(People) << endl;
	cout << sizeof(int) << endl;

	p = new Proer;
	p1 = p;
	p->Level = 10;
	cout << &(p->Level) << endl;

	delete p;
	
	//  delete p;
	// p->GetLevel();

	cout << endl;
	p = new Teaer;
	p2 = p;
	p->Level = 100;
	cout << &(p->Level) << endl;
	/*delete p;*/

	cout << endl;
	cout << "全部析构" << endl;
	p1->GetLevel();
	p2->GetLevel();
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
