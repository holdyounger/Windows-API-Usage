// CmdLineDesensization.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <string>
// #include <unistd.h>  // for sleep()
using namespace  std;
//int main(int argc, char* argv[]) {
int main() {

	string argv;
	getline(cin, argv);

	int argc = 1;

	if (argc != 3) {
		cout << "please input -p and pwd" << endl;
		return 1;
	}
	std::string paramKey = argv;
	std::string pwd; 
	if (paramKey == "-p")
	{
		pwd = argv; //保存密码到内存中，后续程序使用
		//char* argument = argv.c_str();
		//while (*argument) *argument++ = 'x'; //用'x'替换输入的密码
	}

#ifdef solution1
	if (argc != 3) {
		cout << "please input -p and pwd" << endl;
		return 1;
	}
	std::string paramKey = argv[1];
	std::string pwd;
	if (paramKey == "-p") 
	{
		pwd = argv[2]; //保存密码到内存中，后续程序使用
		char* argument = argv[2];
		while (*argument) *argument++ = 'x'; //用'x'替换输入的密码
	}
	// sleep(100);   // 让程序不退出便于观察结果
#endif // solution1
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
