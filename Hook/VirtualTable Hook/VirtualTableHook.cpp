#include <iostream>
#include <windows.h>

class Base {

public:
	virtual  void Print() {
		printf("gyarmy.com\n");
	}

};

void hookPrint() {
	printf("hook_gyarmy\n");
}


int main(int argc, char* argv[])
{
	Base* pb = new Base();

	//关键定位虚表的位置
	DWORD* pVtAddr = (DWORD*)*(DWORD*)pb;

	DWORD dwOldProtect = 0;

	VirtualProtect(pVtAddr, 4, PAGE_EXECUTE_READWRITE, &dwOldProtect);
	*pVtAddr = (DWORD)hookPrint;

	pb->Print();

	delete pb;

	return 0;
}