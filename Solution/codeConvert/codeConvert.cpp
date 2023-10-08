// codeConvert.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include "UriHelper.h"

int main()
{
    std::cout << "Hello World!\n";

	std::string strUrl = "https://10.92.4.117:8443/tac/download/client/com.qianxin.trustagent.win.standard.exe.bm-3.1.1.4585/TrustAgent_standard_3.1.1.4585.aef031d09[10.92.4.117@443].exe";

	auto const pos = strUrl.find_last_of('/');
	const auto fileName = strUrl.substr(pos + 1); // 目标文件名
	std::string url = strUrl.substr(0, pos+1);

	std::string strMsg = url.append(UriHelper::Encode(fileName));


    // 
    std::cout << strMsg << std::endl;
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
