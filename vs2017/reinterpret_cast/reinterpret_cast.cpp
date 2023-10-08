// reinterpret_cast.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <string>
using namespace std;
class A
{
public:
	A();
	A(int n) :i(n), j(n) {}
	~A();

public:
	int i;
	int j;
private:

};

A::A()
{
}

A::~A()
{
}

int main()
{
	cout << "------start------" << endl;

	cout << "<do> A objA(100);" << endl;
	A objA(100);

	int &nNum = reinterpret_cast<int&>(objA);

	cout << "<do> num = 50;" << endl;
	nNum = 50;

	cout << "first <reinterpret_cast>:" << nNum << endl;
	cout << "A.i, A.j = " << objA.i << "," << objA.j << endl;

	cout << "------------------------" << endl;

	long long nNum2 = 300;
	A *pObjA = reinterpret_cast<A*> (&nNum2);
	pObjA->i = 10;
	pObjA->j = 20; // 这条语句会导致内存越界，如果nNum2 申请为longlong就不会越界了

	cout << "pObjA.i,pObjA.j = " << pObjA->i << "," << pObjA->j << endl;
	cout << "nNum2 = " << nNum2 << endl;

	cout << "------end------" << endl;

}