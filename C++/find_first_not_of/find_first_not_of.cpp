// find_first_not_of.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <string>
#include <iostream>


int main() {
	std::string to_search = "Some data with %MACROS to substitute";

	std::cout << "Before: " << to_search << '\n';

	auto pos = std::string::npos;
	while ((pos = to_search.find('%')) != std::string::npos) {
		// 宏名中容许大写字母、小写字母和数字
		const auto after = to_search.find_first_not_of("ABCDEFGHIJKLMNOPQRSTUVWXYZ"
			"abcdefghijklmnopqrstuvwxyz"
			"0123456789", pos + 1);

		// 现在 to_search[pos] == '%' 而 to_search[after] == ' ' （在 'S' 后）

		if (after != std::string::npos)
			to_search.replace(pos, after - pos, "some very nice macros");
	}

	std::cout << "After: " << to_search << '\n';
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
