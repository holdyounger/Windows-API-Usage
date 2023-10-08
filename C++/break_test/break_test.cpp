// break_test.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"


int _tmain(int argc, _TCHAR* argv[])
{

	for(int i = 0; i < 10; i++)
	{
		if(i == 3)
		{
			break;
		}
		printf("%d\n", i);
	}

	return 0;
}

