// vldforptr.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <memory>
#include <stdio.h>
#include <string.h>
// #include <alloc.h>



#define VLD_FORCE_ENABLE
#include <vld.h>

using namespace std;

#define MAX_PATH 260

class A
{
public:
	int i;
	A(int n) :i(n) {
		cout << typeid(this).name() << endl;
	};
	~A() { cout << i << " " << "destructed" << endl; }
};

class B : public A 
{
public:
	int j;
	B(int n) : j(n),A(n) { };
	~B() { cout << j << " " << "destructed" << endl; }
};

int* test_heap_alloc()
{
	int* pTable = new int(256); // 申请一个int结构体变量，初始值为256;
	for (int i = 0; i < 256; i++)
		pTable[i] = i;
	return pTable;
}

int main()
{
	std::cout << "Mermory Leak Test!\n";

	char* ch;

#if 0
	A* p = new A(10);
	B* pB = new B(10);
	shared_ptr <A> sp1(p), sp2(p);

// 	cout << typeid(p).name() << endl;
// 	cout << typeid(pB).name() << endl;

#endif

#if 0
	char* dup_str, * string;
	string = (char*)malloc(sizeof(char) * 10);
	// std::shared_ptr<char> spString(string);
	memset(string, 0, 7);
	sprintf_s(string, 10, "123:%s", "ae");
	dup_str = _strdup(string);    
	printf("%s\n", dup_str);    free(dup_str);

	if (dup_str != NULL)
	{
		free(dup_str);
		dup_str = NULL;
	}
	return 0;
#endif

#if 0
	ch = (char*)malloc(sizeof((int)MAX_PATH));
#endif

#if 0
	// ch = new char[256];
	ch = (char*)malloc(sizeof(8));
	{shared_ptr<char> spCh(ch); }

	memset(ch, 0, 8);

	for (int i = 0; i < 7; i++)
	{
		ch[i] = '1';
	}

	printf("ch:%s\n", ch);
#endif

#if 0
	char* p = (char*)malloc(10);
	char* newp = (char*)realloc(p, 20);        // realloc不能保证newp与p指向同一块内存
	{shared_ptr<char> spCh(newp); }

	if (newp) {
		// p = newp;
	}
	free(p);
#endif

#if 0
	test_heap_alloc();
	// 再申请堆内存会奔溃
	char* p = new char(0); // 申请一个字节，内容初始化为0
#endif


#if 0

	int len;
	int nums[3] = { 2,2,1 };

	len = (int)(sizeof(nums) / sizeof(nums[0]));
	printf("%d\n", (int)(sizeof(nums) / sizeof(nums[0])));

	int* nPtr = nums;
	while (nPtr != nullptr)
	{
		cout << *nPtr << endl;
		nPtr++;
	}
#endif


	string str = "123 123";
	string str1 = "123 123";
	cout << "比较结果：" << str.compare(str1);

	return 0;

}