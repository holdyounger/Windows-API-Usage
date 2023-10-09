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