// CallBackFunc.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <string>
using namespace std;


string toZeroDown(int n, void *contex) {
	cout  << "toZeroDown:" << -*(int*)(contex) << endl;
	return "toZeroDown";
}

string toZeroUp(int n, void *contex) {
	cout << "toZeroUp:" << *(int*)(contex) << endl;
	return "toZeroUp";
}

typedef string(*CallBackFunc)(int n, void *contex);

void registNumCallBack(CallBackFunc callback, void *contex)
{
	int n = 3;
	cout << "The Option of 'callback()' is:" << callback(n, contex) << endl;
}

int main()
{
	for (int i = 0; i < 10; i++)
	{
		if (i % 2) {
			registNumCallBack(toZeroDown, &i);
		}
		else
		{
			registNumCallBack(toZeroUp, &i);
		}
	}
	
}