// bind.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <functional>

double callableFunc(double x, double y = 2) { return x / y; }
int main()
{
	auto NewCallable = std::bind(callableFunc, std::placeholders::_1, std::placeholders::_2);
	std::cout << NewCallable(10,10) << '\n';
}
