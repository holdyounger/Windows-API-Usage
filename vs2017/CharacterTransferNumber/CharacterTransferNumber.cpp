// CharacterTransferNumber.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#define CHECKSUM_MAGIC ((unsigned int)'CKSM')					// 1129009997
#define SYSHASH_MAGIC ((unsigned int)'SHSH')					// 1397248840			// hash
#define INTEGRITY_MAGIC ((unsigned int)'MAGC')					//
#define CHECKSUM_MAGIC ((unsigned int)'CKSM')					//
#define TRUSTFILE_MAGIC	((unsigned int)'TRFL')					//						// trust file(dir)
#define TRUSTPROCESS_MAGIC	((unsigned int)'TRPS')				//						// trust process
#define TRUSTWEAKENPROCESS_MAGIC	((unsigned int)'TRWP')		//						// trust weaken process, it's child process will not have this function
#define PROTECTFILE	((unsigned int)'PRFL')						//						// protected system file(dir)
#define PROTECTFILE_EXCEPTION ((unsigned int )'PRFE')			//						// protected system file exclude
#define PROTECTREG	((unsigned int)'PREG')						//						// protected system register
#define PROTECTREG_K	((unsigned int)'REGK')					//						// protected system register key
#define PROTECTFILE_S	((unsigned int)'PRFS')					//						// protected system file(dir) - from server policy
#define PROTECTREG_S	((unsigned int)'PRES')					//						// protected system register - from server policy
#define PROCESSAUDIT	((unsigned int)'PRAC')					//						// process audit - from client policy
#define PROCESSAUDIT_S	((unsigned int)'PRAS')					//						// process audit - from server policy
#define WORKSHEET_MAGIC ((unsigned int)'WKST')					//
#define HEADER_HASH_MAGIC ((unsigned int)'@#$^')
#define HEADER_FILE_MAGIC ((unsigned int)'^$#@')
#define EMPTY_INPUT ((unsigned int)'ORRE')

#define OBJNAME(A) (#A)<<':'<<(##A)

#define INPUTDATA 0xFEFE
using namespace std;
#pragma warning(disable:4996)

int main()
{
	// cout << INT_MAX << endl;
	std::cout << "输入：" << OBJNAME(INPUTDATA) << endl;
	std::cout << "10进制:" << INPUTDATA << endl;
	std::cout << "16进制:" << std::hex << INPUTDATA << endl;
	std::cout << "-------------------------------------" << endl;
	__int64 flag = INPUTDATA;
	char str[4] = { 0 };
	char * array = (char *)&flag;
	char buff[9] = { 0 };
	strncpy(buff, array, sizeof flag);
	cout << "转换结果：" << buff << endl;
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
