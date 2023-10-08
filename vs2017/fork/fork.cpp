// fork.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
void sig_usr(int signo)
{
	printf("收到了SIGUSR1信号, 进程id= %d\n", getpid());
}
int main(int argc, char *const *argv)
{
	pid_t pid;
	printf("非常高兴和大家一起学习C++\n");
	//系统函数,设置某个信号来的时候处理程序（用哪个函数来处理）
	if (signal(SIGUSR1, sig_usr) == SIG_ERR)
	{
		printf("无法捕捉SIGUSR1信号\n");
		exit(1);
	}
	//fork()之前的代码父进程和子进程都会执行
	pid = fork();   //系统函数，用来创建新进程
	//判断进程是否创建成功
	if (pid < 0)
	{
		printf("fork()进程出错\n");
		exit(1);
	}
	//子进程和父进程都开始执行
	while (1)
	{
		sleep(1);
		printf("休息一秒，进程ID = %d\n", getpid());
	}

	printf("程序退出\n");
}
