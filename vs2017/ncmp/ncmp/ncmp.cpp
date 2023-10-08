// ncmp.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define ARRAY_LENGTH    6
unsigned char a[ARRAY_LENGTH];
unsigned char b[ARRAY_LENGTH];


int main()
{

	unsigned char *c = a;
	for (int i = 0; i < ARRAY_LENGTH; i++)
	{
		a[i] = '1';
		b[i] = '1';
	}
	printf("%d\n", (a == c));
	printf("%d\n", strcmp((const char*)a, (const char *)b));
	printf("%d\n", strncmp((const char*)a, (const char*)b, ARRAY_LENGTH));
	printf("%d\n", memcmp(a, b, ARRAY_LENGTH));

	printf("%s\n", a);
	printf("%s", b);

	printf("over\n");
	char s[] = "1234";

	char ch1[] = "123456";

	char ch2[] = "123456";

	if (ch1 == ch2)
	{
		printf("same");
	}
	else
	{
		printf("diff:ch1=%p,ch2=%p",ch1,ch2);
	}
	printf("%d",(ch1==ch2));

	return 0;

	for (int i = 1; i < strlen(ch2); i++)
	{
		int nCmp = strncmp(ch1, ch2, i);
		printf("%d\n", nCmp);
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
