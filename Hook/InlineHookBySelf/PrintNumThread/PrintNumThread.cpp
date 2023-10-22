#include <iostream>
#include <windows.h>
using namespace std;

static void PrintFun(int nums)
{
	cout << "nums:" << nums << endl;
}

int main()
{
	// ��ȡ������ַ  
	auto function_ptr = std::addressof(PrintFun);

	// ��ӡ������ַ  
	std::cout << "Function address: " << function_ptr << std::endl;

	void(*FunctionAddr) = &PrintFun;

	printf("������ַ��%p\n", FunctionAddr);

	cout << hex << PrintFun  << dec << endl << endl;

	int i = 0;
	while (i++ < 1000)
	{
		PrintFun(i);

		Sleep(1000);
	}
}