// libInput.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

// lib_link_input_2.cpp
// compile by using: cl /EHsc lib_link_input_1.lib lib_link_input_2.cpp


#include <iostream>
#include "../lib_link_input_1/framework.h"
#include "../lib_link_input_1/pch.h"
#include <pch.h>
#ifdef __cplusplus
extern "C" {
#endif

__declspec(dllimport) int Test();

#ifdef __cplusplus
	}
#endif

int main() {
	std::cout << Test() << std::endl;
}
// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单
