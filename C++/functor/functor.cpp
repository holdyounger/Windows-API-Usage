// functor.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <set>
#include <algorithm>
#include "UseLessClass.h"
using namespace std;

class CompareObject {
public:
	void operator()() {
		cout << "仿函数" << endl;
	}

	void operator()(int number, int number2) {
		cout << "仿函数" << endl;
	}
};

// 查看c++ for_each源码自定义
class ShowActionObj {
public:
	void operator()(int content) {
		cout << "custom 仿函数" << content << endl;
	}
};

// 回调方式
void showAction(int content) {
	cout << "custom 普通函数" << content << endl;

}

class Compare
{
public:
	bool operator()(int a, int b)
	{
		return a < b;
	}
};
template <class Function>
void comp(int a, int b, Function func)
{
	if (func(a, b))
	{
		cout << b << " is bigger than " << a << endl;
	}
	else
	{
		cout << a << " is bigger than " << b << endl;
	}
}

void testCompare()
{
	int a = 6;
	int b = 2;
	Compare com;
	comp(a, b, com);
}

void testFunctor()
{
	CompareObject fun1;
	fun1();
}

void testCmp()
{
	set<int> setVar{ 10,20,30,40,50,60 };

	cout << "使用仿函数:" << endl;
	for_each(setVar.begin(), setVar.end(), ShowActionObj());

	cout << "使用普通函数" << endl;
	for_each(setVar.begin(), setVar.end(), showAction);
}

void testLess()
{
	less<UseLessClass> lessClass;
	
	UseLessClass sortClass[3];
	sortClass[0].x = 2;
	sortClass[1].x = 1;
	sortClass[2].x = 3;	
	
	sortClass[0].y = 0;
	sortClass[1].y = 1;
	sortClass[2].y = 2;

	sort(sortClass, sortClass + 2, lessClass);

	for_each(sortClass, sortClass + 3, [] (UseLessClass obj) {
		cout << obj.y << ":" << obj.x << endl; 
		});

	sortClass;
}

int main() 
{
	testLess();


	return 0;
}