// ShellExecute.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <windows.h>
#define CLIENTINSTALL			L" /S -noshortcut=yes"

using namespace std;
int main()
{
    std::cout << "Hello World!\n";
    HINSTANCE hProg = 0;

	std::wstring wstrDest = L"E:\\Users\\shimingming\\Downloads\\591";

	// 添加判断，如果后缀不是exe，则添加后缀
	if (wstrDest.substr(wstrDest.find_last_of(L".") + 1) != L"exe")
	{
		wstrDest.append(L".exe");
	}

	int i = 0;
	while (i++ < 4)
	{

		hProg = ::ShellExecuteW(NULL, L"open", wstrDest.c_str(), CLIENTINSTALL, NULL, SW_SHOW);

		if ((int)hProg <= 32)
		{
			cout << "[INSTALL] test No." << i << " install package failed: " << (int)hProg;
			continue;
		}
		else
		{
			cout << "[INSTALL]" << " install package success: " << (int)hProg;
			i = 4;
		}
	}
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
