// lib_link_input_1.cpp : 定义静态库的函数。
//

#include "pch.h"
#include "framework.h"
#include <windows.h>

// lib_link_input_1.cpp
// compile by using: cl /LD lib_link_input_1.cpp
#ifdef __cplusplus
extern "C" {
#endif

__declspec(dllexport) int Test1()
{
	return 1234;
}

__declspec(dllexport) int Test() {
	return 213;
}

#ifdef __cplusplus
}
#endif