// GetWindowsStatus.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//
//  Sample output:
//  There is       51 percent of memory in use.
//  There are 2029968 total KB of physical memory.
//  There are  987388 free  KB of physical memory.
//  There are 3884620 total KB of paging file.
//  There are 2799776 free  KB of paging file.
//  There are 2097024 total KB of virtual memory.
//  There are 2084876 free  KB of virtual memory.
//  There are       0 free  KB of extended memory.

#include <windows.h>
#include <stdio.h>
#include <tchar.h>

// Use to convert bytes to KB
#define DIV 1024

// Specify the width of the field in which to print the numbers. 
// The asterisk in the format specifier "%*I64d" takes an integer 
// argument and uses it to pad and right justify the number.
#define WIDTH 7

void _tmain()
{
  MEMORYSTATUSEX statex;

  statex.dwLength = sizeof (statex);

  GlobalMemoryStatusEx (&statex);

  double dMemoryLoad = statex.dwMemoryLoad;
  double dTotal = statex.ullTotalPhys / DIV / DIV;
  double dAvailPhys = statex.ullAvailPhys / DIV / DIV;
  double dTotalPageFile = statex.ullTotalPageFile / DIV / DIV;
  double dAvailPageFile = statex.ullAvailPageFile / DIV / DIV;
  double dTotalVirtual = statex.ullTotalVirtual / DIV / DIV;
  double dAvailVirtual = statex.ullAvailVirtual / DIV / DIV;

  printf("There is  %.2f percent of memory in use.\n", dMemoryLoad);
  printf("There is  %.2f total GB of physical memory.\n", dTotal / DIV);
  printf("There is  %.2f free GB of physical memory.\n", dAvailPhys / DIV);

  printf("There is  %.2f total GB of paging file.\n", dTotalPageFile / DIV);
  printf("There is  %.2f free GB of paging file.\n", dAvailPageFile / DIV);
  printf("There is  %.2f total GB of virtual memory.\n", dTotalVirtual / DIV);
  printf("There is  %.2f free  GB of virtual memory.\n", dAvailVirtual / DIV);

  printf("\n");
  _tprintf (TEXT("There is %*ld percent of memory in use.\n"),
            WIDTH, statex.dwMemoryLoad);
  _tprintf (TEXT("There are %*I64d total GB of physical memory.\n"),
            WIDTH, statex.ullTotalPhys / DIV);
  _tprintf (TEXT("There are %*I64d free  KB of physical memory.\n"),
            WIDTH, statex.ullAvailPhys/DIV);

  _tprintf (TEXT("There are %*I64d total KB of paging file.\n"),
            WIDTH, statex.ullTotalPageFile/DIV);
  _tprintf (TEXT("There are %*I64d free  KB of paging file.\n"),
            WIDTH, statex.ullAvailPageFile/DIV);
  _tprintf (TEXT("There are %*I64d total KB of virtual memory.\n"),
            WIDTH, statex.ullTotalVirtual/DIV);
  _tprintf (TEXT("There are %*I64d free  KB of virtual memory.\n"),
            WIDTH, statex.ullAvailVirtual/DIV);

  // Show the amount of extended memory available.

  _tprintf (TEXT("There are %*I64d free  KB of extended memory.\n"),
            WIDTH, statex.ullAvailExtendedVirtual/DIV);
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
