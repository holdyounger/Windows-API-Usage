// ReadSamReg.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>

#include "kuhl_m_lsadump.h"

int main(int argc, wchar_t* argv[])
{
	getchar();

	kuhl_m_lsadump_sam(argc, argv);

	system("pause");

	return 0;
}