// lambda.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <functional>
using namespace std;

class lambdaTest
{
public:
	void output(int x, int y)
	{
		//auto x1 = [] {return m_number; };						// error

		auto x2 = [=] {return m_number + x + y; };				// ok
		// auto x3 = [=] { x = 2; return m_number + x + y; };	// error

		auto x4 = [&] {return m_number + x + y; };				// ok

		auto x5 = [this] { this->m_number = 20; return m_number; };					// ok
		//auto x6 = [this] {return m_number + x + y; };			// error
		auto x7 = [this, x, y] { this->m_number = 2; return m_number + x + y; };		// ok
		auto x8 = [this] {return m_number++; };					// ok
	}

	int m_number = 100;
};

void lambdaFunc()
{
	auto add1 = [](int x, int y) -> int { return x + y;	};
	auto add2 = [](int x, int y) -> decltype(x + y) { return x + y;	};
	auto add3 = [](int a, int b) -> auto { return a + b; };

	auto modify_add = [](int a, int b)  mutable -> auto { a = 3; return a + b; };
	auto value_modify_add = [=](int a, int b)  mutable -> auto { a = 4; return a + b; };
	auto refrence_modify_add = [&](int a, int b)  mutable -> auto { a = 5; return a + b; };
	auto refrence_modify_add_byRef = [&](int& a, int b)  mutable -> auto { a = 6; return a + b; };


	int x = 2, y = 2;

	cout << "add1:" << add1(x, y) << endl;
	cout << "add2:" << add2(x, y) << endl;
	cout << "add3:" << add3(x, y) << endl;

	cout << "modify_add:" << modify_add(x, y) << endl;
	cout << "x=" << x << endl;
	cout << "value_modify_add:" << value_modify_add(x, y) << endl;
	cout << "x=" << x << endl;
	cout << "value_modify_add:" << refrence_modify_add(x, y) << endl;
	cout << "x=" << x << endl;
	cout << "refrence_modify_add_byRef:" << refrence_modify_add_byRef(x, y) << endl;
	cout << "x=" << x << endl;

}

void testLambdawithfunction()
{
	// 包装可调用函数
	std::function<int(int)> f1 = [](int a) {return a; };
	// 绑定可调用函数
	std::function<int(int)> f2 = bind([](int a) {return a; }, placeholders::_1);

	// 绑定可修改的函数
	std::function<int(int&, int)> f3 = [](int& a, int b) { a = 3; return a + b; };
	// 绑定可修改的函数
	std::function<int(int&, int)> f4 = bind([](int& a, int b) { a = 4; return a + b; }, std::placeholders::_1, std::placeholders::_2);


	int x = 2, y = 2;

	// 函数调用
	cout << "f1(100)" << f1(100) << endl;
	cout << "f2(200)" << f2(200) << endl;
	cout << "f3(x, y)" << f3(x, y) << endl;
	cout << "x=" << x << endl;
	cout << "f3(x, y)" << f4(x, y) << endl;
	cout << "x=" << x << endl;
}

int main()
{
    std::cout << "Hello World!\n";

	lambdaTest test;

	test.output(1,2);

	lambdaFunc();

	testLambdawithfunction();
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
