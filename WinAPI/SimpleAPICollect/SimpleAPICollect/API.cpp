#include "API.h"

int InterlockedCompareExchange_test()
{
	using std::cout;
	using std::endl;
	unsigned int des = 3;
	unsigned int a = 2, b = 3;

	cout << ::InterlockedCompareExchange(&des, a, b) << endl;
	cout << des << endl;
	cout << a << endl;
	cout << b << endl;
	return 0;

	/*
	3
	2
	2
	3
	*/
}