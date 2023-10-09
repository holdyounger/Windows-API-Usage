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

#include "GetWindowsStatus.h"
#include <windows.h>
#include <stdio.h>
#include <tchar.h>

// Use to convert bytes to KB
#define DIV 1024

// Specify the width of the field in which to print the numbers. 
// The asterisk in the format specifier "%*I64d" takes an integer 
// argument and uses it to pad and right justify the number.
#define WIDTH 7

void GetWindowsStatus()
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
