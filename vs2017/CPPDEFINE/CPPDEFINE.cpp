#include <iostream>
using namespace std;

int gFont[] = {346,256,213,344,275,223};

int main()
{
	// system("systeminfo | findstr \"[OS处理器]\"");
	for (int i = 0; i < _countof(gFont); i++)
	{
		printf("%x ", gFont[i]);
	}
	printf("\n");
}